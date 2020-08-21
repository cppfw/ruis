#include "inflater.hpp"

#include "context.hpp"

#include "util/util.hpp"

using namespace morda;

inflater::inflater(morda::context& context):
		context(context)
{}

void inflater::add_factory(std::string&& widgetName, decltype(factories)::value_type::second_type&& factory){
	auto ret = this->factories.insert(std::make_pair(
			std::move(widgetName),
			std::move(factory)
		));
	if(!ret.second){
		throw std::logic_error("Failed registering widget type, widget type with given name is already added");
	}
}

bool inflater::unregister_widget(const std::string& widgetName)noexcept{
	if(this->factories.erase(widgetName) == 0){
		return false;
	}
	return true;
}

std::shared_ptr<morda::widget> inflater::inflate(const papki::file& fi) {
	return this->inflate(puu::read(fi));
}

namespace{
const char* wording_defs = "defs";
}

namespace{
void substitute_vars(
		puu::forest& to,
		const std::function<const puu::forest*(const std::string&)>& find_var,
		bool blank_not_found_vars,
		bool go_beyond_child_widgets
	)
{
	ASSERT(find_var)

	for(auto i = to.begin(); i != to.end();){
		// TRACE(<< "i->value = " << i->value.to_string() << std::endl)
		if(i->value == "$"){
			if(i->children.empty()){
				throw std::invalid_argument("malformed GUI definition: error: reference to a variable holds no variable name");
			}

			if(i->children.size() != 1){
				throw std::invalid_argument("malformed GUI definition: reference to variable holds more than one variable name");
			}

			if(!i->children.front().children.empty()){
				throw std::invalid_argument("malformed GUI definition: variable name has children");
			}

			if(auto var = find_var(i->children.front().value.to_string())){
				i = to.insert(i, var->begin(), var->end());
				i = std::next(i, std::distance(var->begin(), var->end()));
				i = to.erase(i);
				continue;
			}

			if(blank_not_found_vars){
				i = to.erase(i);
				continue;
			}
		}else{
			if(i->value.empty() || go_beyond_child_widgets || i->value[0] != '@'){
				substitute_vars(i->children, find_var, blank_not_found_vars, true);
			}
		}
		++i;
	}
}
}

namespace{
puu::forest apply_gui_template(const puu::forest& templ, const std::set<std::string>& var_names, puu::forest&& trees){
	// TRACE(<< "applying template: " << puu::to_string(templ) << std::endl)
	puu::forest ret = templ;

	std::map<std::string, puu::forest> vars;
	puu::forest children;
	for(auto& i : trees){
		if(!is_leaf_property(i.value)){
			children.emplace_back(std::move(i));
			continue;
		}

		if(var_names.find(i.value.to_string()) != var_names.end()){
			vars[i.value.to_string()] = std::move(i.children);
			continue;
		}

		auto t = std::find(ret.begin(), ret.end(), i.value);
		if(t != ret.end()){
			t->children = std::move(i.children);
			continue;
		}
	
		ret.emplace_back(std::move(i));
	}
	vars["children"] = std::move(children);

// #ifdef DEBUG
// 	for(auto& v : vars){
// 		TRACE(<< "v = " << v.first << std::endl)
// 	}
// #endif
// 	TRACE(<< "ret = " << puu::to_string(ret) << std::endl)
	substitute_vars(
			ret,
			[&vars](const std::string& name) -> const puu::forest*{
				// TRACE(<< "looking for var = " << name << std::endl)
				auto i = vars.find(name);
				if(i == vars.end()){
					// TRACE(<< "not found" << std::endl)
					return nullptr;
				}
				// TRACE(<< "found = " << puu::to_string(i->second) << std::endl)
				return &i->second;
			},
			false,
			true
		);

	return ret;
}
}

const decltype(inflater::factories)::value_type::second_type& inflater::find_factory(const std::string& widgetName) {
	auto i = this->factories.find(widgetName);

	if(i == this->factories.end()){
		std::stringstream ss;
		ss << "inflater::find_factory(): widget name '" << widgetName << "' not found";
		throw std::logic_error(ss.str());
	}

	return i->second;
}

std::shared_ptr<widget> inflater::inflate(const char* str){
	return this->inflate(puu::read(str));
}

std::shared_ptr<widget> inflater::inflate(const std::string& str){
	return this->inflate(str.c_str());
}

std::shared_ptr<widget> inflater::inflate(puu::forest::const_iterator begin, puu::forest::const_iterator end){
	auto i = begin;

	for(; i != end && is_leaf_property(i->value); ++i){
		// TRACE(<< "inflater::inflate(): i->value = " << i->value.to_string() << std::endl)
		if(i->value == wording_defs){
			this->push_defs(i->children);
		}else{
			throw std::invalid_argument("inflater::inflate(): unknown declaration encountered before first widget");
		}
	}

	if(i == end){
		return nullptr;
	}

	ASSERT_INFO(!is_leaf_property(i->value), "i->value = " << i->value.to_string())
	ASSERT(!i->value.empty())
	ASSERT(i->value.to_string()[0] == '@')

	std::string widget_name;
	puu::forest widget_desc;

	// TRACE(<< "inflating = " << i->value.to_string() << std::endl)
	if(auto tmpl = this->find_template(i->value.to_string().substr(1))){
		widget_name = tmpl->templ.value.to_string().substr(1);
		widget_desc = apply_gui_template(tmpl->templ.children, tmpl->vars, puu::forest(i->children));
		// TRACE(<< "After applying template: " << puu::to_string(widget_desc) << std::endl)
	}else{
		widget_name = i->value.to_string().substr(1);
		widget_desc = i->children;
	}

	auto fac = this->find_factory(widget_name);

	unsigned num_pop_defs = 0;
	utki::scope_exit pop_defs_scope_exit([this, &num_pop_defs](){
		for(unsigned i = 0; i != num_pop_defs; ++i){
			this->pop_defs();
		}
	});

	for(auto& d : widget_desc){
		if(d.value != wording_defs){
			continue;
		}
		// TRACE(<< "push local defs: " << puu::to_string(d.children) << std::endl)
		this->push_defs(d.children);
		++num_pop_defs;
	}

	substitute_vars(
			widget_desc,
			[this](const std::string& name) -> const puu::forest*{
				return this->find_variable(name);
			},
			true,
			false
		);
	
	try{
		return fac(utki::make_shared_from(this->context), widget_desc);
	}catch(...){
		// TRACE(<< "could not inflate widget: " << widget_name << "{" << puu::to_string(widget_desc) << "}" << std::endl)
		throw;
	}
}

inflater::widget_template inflater::parse_template(const puu::forest& templ){
	// TRACE(<< "parse_template(): templ = " << puu::to_string(templ) << std::endl)
	widget_template ret;

	for(auto& n : templ){
		// template definition
		if(!ret.templ.value.empty()){
			break;
		}

		if(is_leaf_property(n.value)){
			// possibly variable name
			if(!n.children.empty()){
				throw std::invalid_argument("inflater::parse_template(): template argument name has children");
			}
			ret.vars.insert(n.value.to_string());
			continue;
		}

		ASSERT(!is_leaf_property(n.value))
		// TRACE(<< "template name = " << n.value.to_string() << std::endl)

		ret.templ = n;
	}
	if(ret.templ.value.empty()){
		throw std::invalid_argument("inflater::parse_template(): template has no definition");
	}
	// TRACE(<< "template definition found: " << puu::to_string(ret.templ) << std::endl)

	// TRACE(<< "vars:" << std::endl)
	// for each variable create a stub property if needed
	for(auto& v : ret.vars){
		// TRACE(<< " " << v << std::endl)
		auto i = std::find(ret.templ.children.begin(), ret.templ.children.end(), v);
		if(i == ret.templ.children.end()){
			ret.templ.children.push_back(
					// puu::tree(puu::leaf(v), {puu::tree()}) // TODO: is empty child needed?
					puu::tree(puu::leaf(v))
				);
		}
	}

	ASSERT(ret.templ.value.to_string()[0] == '@')

	if(auto tmpl = this->find_template(ret.templ.value.to_string().substr(1))){
		ret.templ.value = tmpl->templ.value;
		ret.templ.children = apply_gui_template(tmpl->templ.children, tmpl->vars, std::move(ret.templ.children));

		ret.vars.insert(tmpl->vars.begin(), tmpl->vars.end()); // forward all variables
	}

	// TRACE(<< "template parsed: " << puu::to_string(ret.templ) << std::endl)
	return ret;
}

void inflater::push_defs(const puu::forest& chain) {
	this->push_variables(chain);
	this->push_templates(chain);
}

void inflater::pop_defs() {
	this->pop_variables();
	this->pop_templates();
}

void inflater::push_templates(const puu::forest& chain){
	decltype(this->templates)::value_type m;

	for(auto& c : chain){
		if(is_property(c)){
			continue;
		}

		if(c.children.empty()){
			throw std::invalid_argument("inflater::push_templates(): template name has no children, error.");
		}
		// TRACE(<< "pushing template: " << puu::to_string(c.children) << std::endl)

		ASSERT(!c.value.empty())
		ASSERT(c.value.to_string()[0] == '@')

		auto res = m.insert(
				std::make_pair(c.value.to_string().substr(1), parse_template(c.children))
			);

		if(!res.second){
			throw std::invalid_argument("inflater::push_templates(): template name is already defined in given templates chain, error.");
		}
	}

	this->templates.push_back(std::move(m));
}

void inflater::pop_templates(){
	ASSERT(this->templates.size() != 0)
	this->templates.pop_back();
}

const inflater::widget_template* inflater::find_template(const std::string& name)const{
	for(auto i = this->templates.rbegin(); i != this->templates.rend(); ++i){
		auto r = i->find(name);
		if(r != i->end()){
			return &r->second;
		}
	}
//	TRACE(<< "inflater::find_template(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}

const puu::forest* inflater::find_variable(const std::string& name)const{
	for(auto i = this->variables.rbegin(); i != this->variables.rend(); ++i){
		auto r = i->find(name);
		if(r != i->end()){
			return &r->second;
		}
	}
//	TRACE(<< "inflater::find_variable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}

void inflater::pop_variables(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_back();
}

void inflater::push_variables(const puu::forest& defs){
	decltype(this->variables)::value_type m;

	for(auto& t : defs){
		if(!is_property(t)){
			continue;
		}

		auto value = t.children;

		substitute_vars(
			value,
			[this](const std::string& name) -> const puu::forest*{
				return this->find_variable(name);
			},
			true,
			true
		);

		if(!m.insert(
				std::make_pair(t.value.to_string(), std::move(value))
			).second)
		{
			std::stringstream ss;
			ss << "inflater::push_variables(): failed to add variable '" << t.value.to_string() << "', variable with same name is already defined in this defs block";
			throw std::logic_error(ss.str());
		}
	}

	this->variables.push_back(std::move(m));
}
