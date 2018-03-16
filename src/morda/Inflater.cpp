#include "Inflater.hpp"

#include "widgets/Container.hpp"

#include "widgets/group/LinearContainer.hpp"
#include "widgets/group/Pile.hpp"
#include "widgets/group/ScrollArea.hpp"
#include "widgets/group/Table.hpp"
#include "widgets/group/TableRow.hpp"
#include "widgets/group/Overlay.hpp"
#include "widgets/group/List.hpp"

#include "widgets/proxy/KeyProxy.hpp"
#include "widgets/proxy/MouseProxy.hpp"
#include "widgets/proxy/ResizeProxy.hpp"

#include "Morda.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->registerType<Widget>("Widget");
	this->registerType<Container>("Container");
	this->registerType<Row>("Row");
	this->registerType<Column>("Column");
	this->registerType<Pile>("Pile");
	this->registerType<MouseProxy>("MouseProxy");
	this->registerType<ScrollArea>("ScrollArea");
	this->registerType<Table>("Table");
	this->registerType<TableRow>("TableRow");
	this->registerType<KeyProxy>("KeyProxy");
	this->registerType<Overlay>("Overlay");
	this->registerType<ResizeProxy>("ResizeProxy");
	this->registerType<HorizontalList>("HorizontalList");
	this->registerType<VerticalList>("VerticalList");
}



void Inflater::addWidgetFactory(const std::string& widgetName, decltype(widgetFactories)::value_type::second_type factory){
	auto ret = this->widgetFactories.insert(std::make_pair(
			widgetName,
			std::move(factory)
		));
	if(!ret.second){
		throw Inflater::Exc("Failed registering widget type, widget type with given name is already added");
	}
}



bool Inflater::removeWidget(const std::string& widgetName)noexcept{
	if(this->widgetFactories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



std::shared_ptr<morda::Widget> Inflater::inflate(papki::File& fi) {
	std::unique_ptr<stob::Node> root = this->load(fi);
	ASSERT(root)

	return this->inflate(*root);
}


namespace{
const char* defs_c = "defs";
}

namespace{
void substituteVars(stob::Node* to, const std::function<const stob::Node*(const std::string&)>& findVar){
	if(!to || !findVar){
		return;
	}
	
	for(; to;){
		if(*to == "@"){
			if(!to->child()){
				throw Exc("malformed GUI definition: error: reference to a variable holds no variable name");
			}

			const auto name = to->child();

			if(name->next()){
				throw Exc("malformed GUI definition: reference to variable holds more than one variable name");
			}

			if(name->child()){
				throw Exc("malformed GUI definition: variable name has children");
			}

			if(auto var = findVar(name->value())){
				auto next = to->next();
				to->replace(*var);
				to = next;
				continue;
			}
		}else{
			if(to->child()){
				substituteVars(to->child(), findVar);
			}
		}
		to = to->next();
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
	std::map<std::string, std::unique_ptr<stob::Node>> vars;
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
				vars[chain->value()] = chain->removeChildren();
				chain = chain->chopNext();
				continue;
			}else if(ret){
				if(auto n = ret->thisOrNext(chain->value()).node()){
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
		vars["children"] = childrenChain->chopNext();
	}
	
	substituteVars(
			ret.get(),
			[&vars](const std::string& name) -> const stob::Node*{
//				TRACE(<< "looking for var = " << name << std::endl)
				auto i = vars.find(name);
				if(i == vars.end()){
//					TRACE(<< "not found" << std::endl)
					return nullptr;
				}
//				TRACE(<< "found = " << i->second.get() << std::endl)
				return i->second.get();
			}
		);
	
	return ret;
}
}

const decltype(Inflater::widgetFactories)::value_type::second_type* Inflater::findFactory(const std::string& widgetName) {
	auto i = this->widgetFactories.find(widgetName);

	if(i == this->widgetFactories.end()){
		return nullptr;
	}
	
	return &i->second;
}


std::shared_ptr<morda::Widget> Inflater::inflate(const char* str){
	return this->inflate(*stob::parse(str));
}


std::shared_ptr<morda::Widget> Inflater::inflate(const stob::Node& chain){
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
			throw Exc("Inflater::Inflate(): unknown declaration encountered before first widget");
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
	
	
	auto fac = this->findFactory(n->value());
	
	if(!fac){
		TRACE(<< "Inflater::Inflate(): n->value() = " << n->value() << std::endl)
		std::stringstream ss;
		ss << "Failed to inflate, no matching factory found for requested widget name: " << n->value();
		throw Exc(ss.str());
	}

	bool needPopDefs = false;
	utki::ScopeExit scopeExit([this, &needPopDefs](){
		if(needPopDefs){
			this->popDefs();
		}
	});
	
	if(auto v = n->child(defs_c).node()){
		if(v->child()){
			this->pushDefs(*v->child());
			needPopDefs = true;
		}
	}

	if(cloned){
		cloned = cloned->removeChildren();
	}else{
		if(n->child()){
			cloned = n->child()->cloneChain();
		}
	}
	
	this->substituteVariables(cloned.get());

	return fac->operator()(cloned.get());
}



std::unique_ptr<stob::Node> Inflater::load(papki::File& fi){
	std::unique_ptr<stob::Node> ret = stob::load(fi);
	
	ret = std::move(std::get<0>(resolveIncludes(fi, std::move(ret))));

	return ret;
}

Inflater::Template Inflater::parseTemplate(const stob::Node& chain){
	Template ret;
	
	for(auto n = &chain; n; n = n->next()){
		if(n->isProperty()){
			//possibly variable name
			if(n->child()){
				throw Exc("malformed GUI declaration: template argument name has children");
			}
			ret.vars.insert(n->value());
			continue;
		}
		//template definition
		if(ret.t){
			continue;
		}
		ret.t = n->clone();
	}
	if(!ret.t){
		throw Exc("malformed GUI declaration: template has no definition");
	}
	ASSERT(ret.t)
	
	//for each variable create a stub property if needed
	for(auto& v : ret.vars){
		auto p = ret.t->child(v.c_str()).node();
		if(!p){
			ret.t->addAsFirstChild(v.c_str());
			ret.t->child()->addAsFirstChild(nullptr);
		}
	}
	
//	TRACE(<< "stubbed template = " << ret.t->chainToString(true) << std::endl)
	
	if(auto tmpl = this->findTemplate(ret.t->value())){
		ret.t->setValue(tmpl->t->value());
		ret.t->setChildren(mergeGUIChain(tmpl->t->child(), tmpl->vars, ret.t->removeChildren()));
		ret.vars.insert(tmpl->vars.begin(), tmpl->vars.end());//forward all variables
	}
	
//	TRACE(<< "parsed template = " << ret.t->chainToString(true) << std::endl)
	return ret;
}

void Inflater::pushDefs(const stob::Node& chain) {
	this->pushVariables(chain);
	this->pushTemplates(chain);
}

void Inflater::popDefs() {
	this->popVariables();
	this->popTemplates();
}



void Inflater::pushTemplates(const stob::Node& chain){
	decltype(this->templates)::value_type m;
	
	for(auto c = &chain; c; c = c->next()){
		if(c->isProperty()){
			continue;
		}
		
		if(!c->child()){
			throw Exc("Inflater::pushTemplates(): template name has no children, error.");
		}
//		TRACE(<< "pushing template = " << c->value() << std::endl)
		if(!m.insert(std::make_pair(c->value(), parseTemplate(c->up()))).second){
			throw Exc("Inflater::PushTemplates(): template name is already defined in given templates chain, error.");
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



void Inflater::popTemplates(){
	ASSERT(this->templates.size() != 0)
	this->templates.pop_front();
}



const Inflater::Template* Inflater::findTemplate(const std::string& name)const{
	for(auto& i : this->templates){
		auto r = i.find(name);
		if(r != i.end()){
			return &r->second;
		}
	}
//	TRACE(<< "Inflater::FindTemplate(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



const stob::Node* Inflater::findVariable(const std::string& name)const{
	for(auto& i : this->variables){
		auto r = i.find(name);
		if(r != i.end()){
			return r->second.get();
		}
	}
//	TRACE(<< "Inflater::findVariable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



void Inflater::popVariables(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_front();
}


void Inflater::pushVariables(const stob::Node& chain){
	decltype(this->variables)::value_type m;
	
	for(auto n = &chain; n; n = n->next()){
		if(!n->isProperty()){
			continue;
		}
		
		auto value = n->cloneChildren();
		
		this->substituteVariables(value.get());
		
		if(!m.insert(
				std::make_pair(n->value(), std::move(value))
			).second)
		{
			throw morda::Exc("Inflater::pushDefinitions(): failed to add variable, variable with same name is already defined in this variables block");
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

void Inflater::substituteVariables(stob::Node* to)const{
	substituteVars(
			to,
			[this](const std::string& name) -> const stob::Node*{
				return this->findVariable(name);
			}
		);
}
