#include "inflater.hpp"

#include "widgets/container.hpp"

#include "widgets/group/Column.hpp"
#include "widgets/group/Row.hpp"
#include "widgets/group/Pile.hpp"
#include "widgets/group/ScrollArea.hpp"
#include "widgets/group/SizeContainer.hpp"
#include "widgets/group/Overlay.hpp"
#include "widgets/group/List.hpp"

#include "widgets/proxy/KeyProxy.hpp"
#include "widgets/proxy/MouseProxy.hpp"
#include "widgets/proxy/ResizeProxy.hpp"

#include "Morda.hpp"

#include "util/util.hpp"



using namespace morda;



inflater::inflater(){
	this->register_widget<Widget>("Widget");
	this->register_widget<Container>("Container");
	this->register_widget<SizeContainer>("SizeContainer");
	this->register_widget<Row>("Row");
	this->register_widget<Column>("Column");
	this->register_widget<Pile>("Pile");
	this->register_widget<MouseProxy>("MouseProxy");
	this->register_widget<ScrollArea>("ScrollArea");
	this->register_widget<KeyProxy>("KeyProxy");
	this->register_widget<Overlay>("Overlay");
	this->register_widget<ResizeProxy>("ResizeProxy");
	this->register_widget<HList>("HList");
	this->register_widget<VList>("VList");
}



void inflater::add_factory(std::string&& widgetName, decltype(factories)::value_type::second_type&& factory){
	auto ret = this->factories.insert(std::make_pair(
			std::move(widgetName),
			std::move(factory)
		));
	if(!ret.second){
		throw utki::invalid_state("Failed registering widget type, widget type with given name is already added");
	}
}



bool inflater::unregister_widget(const std::string& widgetName)noexcept{
	if(this->factories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



std::shared_ptr<morda::Widget> inflater::inflate(const papki::File& fi) {
	return this->inflate(puu::read(fi));
}


namespace{
const char* defs_c = "defs";
}

namespace{
void substitute_vars(puu::trees& to, const std::function<const puu::trees*(const std::string&)>& findVar){
	if(!findVar){
		return;
	}

	for(auto i = to.begin(); i != to.end();){
		if(i->value == "@" || i->value == "$"){ //TODO: @ is deprecated, remove check for @.

			//TODO: deprecated, remove.
			if(i->value == "@"){
				TRACE_ALWAYS(<< "DEPRACATED: use $ instead of @" << std::endl)
			}

			if(i->children.empty()){
				throw Exc("malformed GUI definition: error: reference to a variable holds no variable name");
			}

			if(i->children.size() != 1){
				throw Exc("malformed GUI definition: reference to variable holds more than one variable name");
			}

			if(!i->children.front().children.empty()){
				throw Exc("malformed GUI definition: variable name has children");
			}

			if(auto var = findVar(i->children.front().value.to_string())){
				if(var->size() != 0){
					i = to.insert(i, var->begin(), var->end());
					i = std::next(i, std::distance(var->begin(), var->end()));
					i = to.erase(i);
					continue;
				}
			}
		}else{
			substitute_vars(i->children, findVar);
		}
		++i;
	}
}
}

namespace{
puu::trees apply_gui_template(const puu::trees& templ, const std::set<std::string>& var_names, puu::trees&& trees){
	puu::trees ret = templ;

	std::map<std::string, puu::trees> vars;
	puu::trees children;
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

	substitute_vars(
			ret,
			[&vars](const std::string& name) -> const puu::trees*{
//				TRACE(<< "looking for var = " << name << std::endl)
				auto i = vars.find(name);
				if(i == vars.end()){
//					TRACE(<< "not found" << std::endl)
					return nullptr;
				}
//				TRACE(<< "found = " << i->second.get() << std::endl)
				return &i->second;
			}
		);

	return ret;
}
}

const decltype(inflater::factories)::value_type::second_type& inflater::find_factory(const std::string& widgetName) {
	auto i = this->factories.find(widgetName);

	if(i == this->factories.end()){
		std::stringstream ss;
		ss << "inflater::find_factory(): widget name '" << widgetName << "' not found";
		throw utki::not_found(ss.str());
	}

	return i->second;
}


std::shared_ptr<widget> inflater::inflate(const char* str){
	return this->inflate(puu::read(str));
}

std::shared_ptr<widget> inflater::inflate(const puu::trees& gui_script){
	return this->inflate(*puu_to_stob(gui_script));
}

std::shared_ptr<morda::Widget> inflater::inflate(const stob::Node& chain){
//	TODO:
//	if(!App::inst().thisIsUIThread()){
//		throw Exc("Inflate called not from UI thread");
//	}

	const stob::Node* n = &chain;
	for(; n && n->isProperty(); n = n->next()){
		if(*n == defs_c){
			if(n->child()){
				this->push_defs(stob_to_puu(*n->child()));
			}
		}else{
			throw Exc("inflater::Inflate(): unknown declaration encountered before first widget");
		}
	}

	if(!n){
		return nullptr;
	}

	ASSERT(!n->isProperty())

	std::unique_ptr<stob::Node> cloned;
//	TRACE(<< "inflating = " << n->value() << std::endl)
	if(auto tmpl = this->find_template(n->value())){
//		TRACE(<< "template name = " << n->value() << std::endl)

		cloned = utki::makeUnique<stob::Node>(tmpl->templ.value.to_string());
		cloned->setChildren(
				puu_to_stob(
						apply_gui_template(tmpl->templ.children, tmpl->vars, stob_to_puu(*n->cloneChildren()))
					)
			);
		n = cloned.get();
//		TRACE(<< "n = " << n->chainToString(true) << std::endl)
	}


	auto fac = this->find_factory(n->value());

	unsigned numPopDefs = 0;
	utki::ScopeExit scopeExit([this, &numPopDefs](){
		for(unsigned i = 0; i != numPopDefs; ++i){
			this->pop_defs();
		}
	});

	for(auto v = n->child(defs_c).get_node(); v; v = v->next(defs_c).get_node()){
		if(v->child()){
			this->push_defs(stob_to_puu(*v->child()));
			++numPopDefs;
		}
	}

	if(cloned){
		cloned = cloned->removeChildren();
	}else{
		if(n->child()){
			cloned = n->child()->cloneChain();
		}
	}

	if(cloned){
		auto c = stob_to_puu(*cloned);

		substitute_vars(
				c,
				[this](const std::string& name) -> const puu::trees*{
					return this->find_variable(name);
				}
			);
		return fac(c);
	}else{
		return fac(puu::trees());
	}
}

inflater::widget_template inflater::parse_template(const puu::trees& templ){
	widget_template ret;

	for(auto& n : templ){
		//template definition
		if(!ret.templ.value.empty()){
			break;
		}

		if(is_leaf_property(n.value)){
			//possibly variable name
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

	// for each variable create a stub property if needed
	for(auto& v : ret.vars){
		auto i = std::find(ret.templ.children.begin(), ret.templ.children.end(), v);
		if(i == ret.templ.children.end()){
			ret.templ.children.push_back(
					puu::tree(puu::leaf(v), {puu::tree()})
				);
		}
	}

	// TRACE(<< "stubbed template = " << ret.t->chainToString(true) << std::endl)

	if(auto tmpl = this->find_template(ret.templ.value.to_string())){
		ret.templ.value = tmpl->templ.value;
		ret.templ.children = apply_gui_template(tmpl->templ.children, tmpl->vars, std::move(ret.templ.children));

		ret.vars.insert(tmpl->vars.begin(), tmpl->vars.end()); // forward all variables
	}

	// TRACE(<< "parsed template = " << ret.t->chainToString(true) << std::endl)
	return ret;
}

void inflater::push_defs(const puu::trees& chain) {
	this->push_variables(chain);
	this->push_templates(chain);
}

void inflater::pop_defs() {
	this->pop_variables();
	this->pop_templates();
}



void inflater::push_templates(const puu::trees& chain){
	decltype(this->templates)::value_type m;

	for(auto& c : chain){
		if(is_property(c)){
			continue;
		}

		if(c.children.empty()){
			throw std::invalid_argument("inflater::pushTemplates(): template name has no children, error.");
		}
//		TRACE(<< "pushing template = " << c->value() << std::endl)

		auto res = m.insert(
				std::make_pair(c.value.to_string(), parse_template(c.children))
			);

		if(!res.second){
			throw Exc("inflater::PushTemplates(): template name is already defined in given templates chain, error.");
		}
	}

	this->templates.push_back(std::move(m));

//#ifdef DEBUG
//	TRACE(<< "Templates Stack:" << std::endl)
//	for(auto& i : this->templates){
//		TRACE(<< "\tTemplates:" << std::endl)
//		for(auto& j : i){
//			TRACE(<< "\t\t" << j.first << " = " << j.second->ChainToString() << std::endl)
//		}
//	}
//#endif
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
//	TRACE(<< "inflater::FindTemplate(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



const puu::trees* inflater::find_variable(const std::string& name)const{
	for(auto i = this->variables.rbegin(); i != this->variables.rend(); ++i){
		auto r = i->find(name);
		if(r != i->end()){
			return &r->second;
		}
	}
//	TRACE(<< "inflater::findVariable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



void inflater::pop_variables(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_back();
}


void inflater::push_variables(const puu::trees& trees){
	decltype(this->variables)::value_type m;

	for(auto& t : trees){
		if(!is_property(t)){
			continue;
		}

		auto value = t.children;

		this->substitute_variables(value);

		if(!m.insert(
				std::make_pair(t.value.to_string(), std::move(value))
			).second)
		{
			std::stringstream ss;
			ss << "inflater::push_variables(): failed to add variable '" << t.value.to_string() << "', variable with same name is already defined in this defs block";
			throw utki::invalid_state(ss.str());
		}
	}

	this->variables.push_back(std::move(m));

//#ifdef DEBUG
//	TRACE(<< "Variables Stack:" << std::endl)
//	for(auto& i : this->variables){
//		TRACE(<< "\tVariables:" << std::endl)
//		for(auto& j : i){
//			TRACE(<< "\t\t" << j.second.first << "|" << j.first << " = " << j.second.second << std::endl)
//		}
//	}
//#endif
}

void inflater::substitute_variables(puu::trees& to)const{
	substitute_vars(
			to,
			[this](const std::string& name) -> const puu::trees*{
				return this->find_variable(name);
			}
		);
}
