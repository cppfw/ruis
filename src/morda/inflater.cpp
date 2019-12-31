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
	return this->inflate(stob_to_puu(*stob::load(fi)));
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
//Merges two STOB chains.
std::unique_ptr<stob::Node> mergeGUIChain(const stob::Node* tmplChain, const std::set<std::string>& varNames, std::unique_ptr<stob::Node> chain){
	if(!chain){
		if(!tmplChain){
			return nullptr;
		}
	}

	std::unique_ptr<stob::Node> ret;
	if(tmplChain){
		ret = tmplChain->cloneChain();
	}

	//prepare variables and remove them from 'chain'
	std::map<std::string, puu::trees> vars;
	{
		auto childrenChain = utki::makeUnique<stob::Node>();
		stob::Node* lastChild = childrenChain.get();
		for(; chain;){
			if(!chain->isProperty()){
				ASSERT(lastChild)
				ASSERT(!lastChild->next())
				ASSERT(chain)
				auto tail = chain->chopNext();
				ASSERT(!chain->next())
				lastChild->insertNext(std::move(chain));
				chain = std::move(tail);
				lastChild = lastChild->next();
				continue;
			}else if(varNames.find(chain->value()) != varNames.end()){
				vars[chain->value()] = stob_to_puu(*chain->removeChildren());
				chain = chain->chopNext();
				continue;
			}else if(ret){
				if(auto n = ret->thisOrNext(chain->value()).get_node()){
					n->setChildren(chain->removeChildren());
					chain = chain->chopNext();
					continue;
				}
			}
			auto tail = chain->chopNext();
			chain->setNext(std::move(ret));
			ret = std::move(chain);
			chain = std::move(tail);
		}
		vars["children"] = stob_to_puu(*childrenChain->chopNext());
	}

	ASSERT(ret)

	auto retret = stob_to_puu(*ret);

	substitute_vars(
			retret,
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

	return puu_to_stob(retret);
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
				this->pushDefs(*n->child());
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
	if(auto tmpl = this->findTemplate(n->value())){
//		TRACE(<< "template name = " << n->value() << std::endl)
		cloned = utki::makeUnique<stob::Node>(tmpl->t->value());
		cloned->setChildren(mergeGUIChain(tmpl->t->child(), tmpl->vars, n->cloneChildren()));
		n = cloned.get();
//		TRACE(<< "n = " << n->chainToString(true) << std::endl)
	}


	auto fac = this->find_factory(n->value());

	unsigned numPopDefs = 0;
	utki::ScopeExit scopeExit([this, &numPopDefs](){
		for(unsigned i = 0; i != numPopDefs; ++i){
			this->popDefs();
		}
	});

	for(auto v = n->child(defs_c).get_node(); v; v = v->next(defs_c).get_node()){
		if(v->child()){
			this->pushDefs(*v->child());
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

inflater::widget_template inflater::parse_template(const stob::Node& chain){
	widget_template ret;

	for(auto n = &chain; n; n = n->next()){
		//template definition
		if(ret.t){
			break;
		}

		if(n->isProperty()){
			//possibly variable name
			if(n->child()){
				throw std::invalid_argument("inflater::parse_template(): template argument name has children");
			}
			ret.vars.insert(n->value());
			continue;
		}
		
		ret.t = n->clone();
	}
	if(!ret.t){
		throw std::invalid_argument("inflater::parse_template(): template has no definition");
	}
	ASSERT(ret.t)

	// for each variable create a stub property if needed
	for(auto& v : ret.vars){
		auto p = ret.t->child(v.c_str()).get_node();
		if(!p){
			ret.t->addAsFirstChild(v.c_str());
			ret.t->child()->addAsFirstChild(nullptr);
		}
	}

	// TRACE(<< "stubbed template = " << ret.t->chainToString(true) << std::endl)

	if(auto tmpl = this->findTemplate(ret.t->value())){
		ret.t->setValue(tmpl->t->value());
		ret.t->setChildren(mergeGUIChain(tmpl->t->child(), tmpl->vars, ret.t->removeChildren()));
		ret.vars.insert(tmpl->vars.begin(), tmpl->vars.end());//forward all variables
	}

	// TRACE(<< "parsed template = " << ret.t->chainToString(true) << std::endl)
	return ret;
}

void inflater::pushDefs(const stob::Node& chain) {
	this->push_variables(stob_to_puu(chain));
	this->push_templates(chain);
}

void inflater::popDefs() {
	this->pop_variables();
	this->pop_templates();
}



void inflater::push_templates(const stob::Node& chain){
	decltype(this->templates)::value_type m;

	for(auto c = &chain; c; c = c->next()){
		if(c->isProperty()){
			continue;
		}

		if(!c->child()){
			throw std::invalid_argument("inflater::pushTemplates(): template name has no children, error.");
		}
//		TRACE(<< "pushing template = " << c->value() << std::endl)
		if(!m.insert(std::make_pair(c->value(), parse_template(c->up()))).second){
			throw Exc("inflater::PushTemplates(): template name is already defined in given templates chain, error.");
		}
	}

	this->templates.push_front(std::move(m));

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
	this->templates.pop_front();
}



const inflater::widget_template* inflater::findTemplate(const std::string& name)const{
	for(auto& i : this->templates){
		auto r = i.find(name);
		if(r != i.end()){
			return &r->second;
		}
	}
//	TRACE(<< "inflater::FindTemplate(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



const puu::trees* inflater::find_variable(const std::string& name)const{
	for(auto& i : this->variables){
		auto r = i.find(name);
		if(r != i.end()){
			return &r->second;
		}
	}
//	TRACE(<< "inflater::findVariable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



void inflater::pop_variables(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_front();
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

	this->variables.push_front(std::move(m));

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
