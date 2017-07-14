#include "Inflater.hpp"

#include "widgets/core/container/Container.hpp"
#include "widgets/core/container/LinearArea.hpp"
#include "widgets/core/container/Frame.hpp"
#include "widgets/core/container/ScrollArea.hpp"
#include "widgets/core/container/Table.hpp"
#include "widgets/core/container/TableRow.hpp"
#include "widgets/core/container/Overlay.hpp"
#include "widgets/core/container/Margins.hpp"

#include "widgets/core/proxy/KeyProxy.hpp"
#include "widgets/core/proxy/MouseProxy.hpp"
#include "widgets/core/proxy/ResizeProxy.hpp"

#include "Morda.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->addWidget<Widget>("Widget");
	this->addWidget<Container>("Container");
	this->addWidget<HorizontalArea>("HorizontalArea");
	this->addWidget<VerticalArea>("VerticalArea");
	this->addWidget<Frame>("Frame");
	this->addWidget<MouseProxy>("MouseProxy");
	this->addWidget<ScrollArea>("ScrollArea");
	this->addWidget<Table>("Table");
	this->addWidget<TableRow>("TableRow");
	this->addWidget<KeyProxy>("KeyProxy");
	this->addWidget<Overlay>("Overlay");
	this->addWidget<ResizeProxy>("ResizeProxy");
	this->addWidget<Margins>("Margins");
}



void Inflater::addWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory){
	std::pair<T_FactoryMap::iterator, bool> ret = this->widgetFactories.insert(
			std::pair<std::string, std::unique_ptr<Inflater::WidgetFactory> >(
					widgetName,
					std::move(factory)
				)
		);
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

const char* templates_c = "templates";
const char* defs_c = "defs";



//Merges two STOB chains. 
std::unique_ptr<stob::Node> mergeGUIChain(const stob::Node* from, std::unique_ptr<stob::Node> to){
	if(!to){
		if(!from){
			return nullptr;
		}
		return from->cloneChain();
	}
	
	std::unique_ptr<stob::Node> children; //children will be stored in reverse order
	
	for(auto src = from; src; src = src->next()){
		if(!src->isProperty()){
			auto c = src->clone();
			c->setNext(std::move(children));
			children = std::move(c);
			continue;
		}
		
		if(!src->child() || *src == "@"){ //@ means reference to a variable
			//No children means that it is a property value, stop further processing of this chain.
			
			//Check that it is the only node in the chain
			if(src != from || src->next()){
				throw Inflater::Exc("malformed gui script: property with several values encountered");
			}
			return to;
		}
		
		auto dst = to->thisOrNext(src->value()).node();
		if(!dst){
			//there is no same named property in 'to', so just clone property there
			to->insertNext(src->clone());
			continue;
		}
		
		if(!dst->child()){
			continue;//no children means that the property is removed in derived template
		}
		
		dst->setChildren(mergeGUIChain(src->child(), dst->removeChildren()));
	}
	
	//add children in reverse order again, so it will be in normal order in the end
	for(; children;){
		auto c = std::move(children);
		children = c->chopNext();
		
		c->setNext(std::move(to));
		to = std::move(c);
	}
	
	return to;
}

}

std::shared_ptr<morda::Widget> Inflater::inflate(const stob::Node& chain){
//	TODO:
//	if(!App::inst().thisIsUIThread()){
//		throw Exc("Inflate called not from UI thread");
//	}
	
	const stob::Node* n = &chain;
	for(; n && n->isProperty(); n = n->next()){
		if(*n == templates_c){
			if(n->child()){
				this->pushTemplates(n->child()->cloneChain());
			}
		}else if(*n == defs_c){
			if(n->child()){
				this->pushDefinitions(*n->child());
			}
		}else{
			throw Exc("Inflater::Inflate(): unknown declaration encountered before first widget");
		}
	}
	
	if(!n){
		return nullptr;
	}
	
	std::unique_ptr<stob::Node> cloned;
	if(auto t = this->findTemplate(n->value())){
		cloned = utki::makeUnique<stob::Node>(t->value());
		cloned->setChildren(mergeGUIChain(t->child(), n->child() ? n->child()->cloneChain() : nullptr));
		n = cloned.get();
	}
	
	
	auto i = this->widgetFactories.find(n->value());

	if(i == this->widgetFactories.end()){
		TRACE(<< "Inflater::Inflate(): n->value() = " << n->value() << std::endl)
		std::stringstream ss;
		ss << "Failed to inflate, no matching factory found for requested widget name: " << n->value();
		throw Exc(ss.str());
	}

	bool needPopTemplates = false;
	bool needPopVariables = false;
	utki::ScopeExit scopeExit([this, &needPopTemplates, &needPopVariables](){
		if(needPopTemplates){
			this->popTemplates();
		}
		if(needPopVariables){
			this->popDefinitions();
		}
	});
	
	if(auto t = n->child(templates_c).node()){
		if(auto c = t->child()){
			this->pushTemplates(c->cloneChain());
			needPopTemplates = true;
		}
	}
	if(auto v = n->child(defs_c).node()){
		if(v->child()){
			this->pushDefinitions(*v->child());
			needPopVariables = true;
		}
	}
	
	{
		if(cloned){
			cloned = cloned->removeChildren();
		}else{
			if(n->child()){
				cloned = n->child()->cloneChain();
			}
		}
		
		this->substituteDefinitions(cloned.get());
		
		return i->second->create(cloned.get());
	}
}



std::unique_ptr<stob::Node> Inflater::load(papki::File& fi){
	std::unique_ptr<stob::Node> ret = stob::load(fi);
	
	ret = std::move(std::get<0>(resolveIncludes(fi, std::move(ret))));

	return ret;
}

void Inflater::pushTemplates(std::unique_ptr<stob::Node> chain){
	decltype(this->templates)::value_type m;
	
	for(; chain; chain = chain->chopNext()){
		if(chain->isProperty()){
			throw Exc("Inflater::PushTemplates(): template name does not start with capital latin letter, error.");
		}
		
		if(chain->child()){
			throw Exc("Inflater::PushTemplates(): template name has children, error.");
		}
		
		if(!chain->next()){
			throw Exc("Inflater::PushTemplates(): template name is not followed by template definition, error.");
		}
		
		if(!m.insert(std::make_pair(chain->value(), chain->removeNext())).second){
			throw Exc("Inflater::PushTemplates(): template name is already defined in given templates chain, error.");
		}
	}
	
	for(auto i = m.begin(); i != m.end(); ++i){
		if(auto s = this->findTemplate(i->second->value())){
			i->second->setValue(s->value());
			ASSERT(s->child())
			i->second->setChildren(mergeGUIChain(s->child(), i->second->removeChildren()));
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



const stob::Node* Inflater::findTemplate(const std::string& name)const{
	for(auto& i : this->templates){
		auto r = i.find(name);
		if(r != i.end()){
			return r->second.get();
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
	TRACE(<< "Inflater::FindVariable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



void Inflater::popDefinitions(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_front();
}



void Inflater::pushDefinitions(const stob::Node& chain){
	decltype(this->variables)::value_type m;
	
	for(auto n = &chain; n; n = n->next()){
		std::unique_ptr<stob::Node> value;
		
		for(auto child = n->child(); child;){
			
			if(*child == "@" && child->child()){
				auto r = child->child();

				if(r->next()){
					throw morda::Exc("Inflater::pushDefinitions(): variable reference has several values, error");
				}

				if (r->child()) {
					throw morda::Exc("Inflater::pushDefinitions(): variable reference has children, error");
				}
				
				if(auto var = this->findVariable(r->value())){
					value = var->cloneChain();
				}else{
					throw morda::Exc("Inflater::pushDefinitions(): variable reference could not be resolved, error");
				}
				
				break;
			}
			
			value = child->cloneChain();
			break;
		}
		
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



void Inflater::substituteDefinitions(stob::Node* to)const{
	if(!to){
		return;
	}
	
	if(*to == "@"){
		if(to->next()){
			throw Exc("Inflater::SubstituteVariables(): error: property value holds something else besides reference to a variable");
		}
		
		if(!to->child()){
			throw Exc("Inflater::SubstituteVariables(): error: reference to a variable holds no variable name");
		}
		
		if(to->child()->next()){
			throw Exc("Inflater::SubstituteVariables(): error: reference to variable holds more than one variable name");
		}
		
		if(to->child()->child()){
			throw Exc("Inflater::SubstituteVariables(): error: variable name has children");
		}
		
		if(auto var = this->findVariable(to->child()->value())){
			//TODO: replace by chain
			to->setValue(var->value());
			if (var->child()) {
				to->setChildren(var->child()->cloneChain());
			}else {
				to->removeChildren();
			}
		}
		
		return;
	}
		
	for(; to; to = to->next()){
		if(to->isProperty() && to->child()){
			this->substituteDefinitions(to->child());
		}
	}
}
