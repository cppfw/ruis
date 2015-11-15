#include "Inflater.hpp"

#include "widgets/containers/Container.hpp"
#include "widgets/containers/BlackHoleContainer.hpp"
#include "widgets/containers/LinearContainer.hpp"
#include "widgets/containers/FrameContainer.hpp"
#include "widgets/containers/ScrollContainer.hpp"
#include "widgets/containers/TableContainer.hpp"
#include "widgets/containers/TableRow.hpp"
#include "widgets/containers/KeyProxyContainer.hpp"
#include "widgets/containers/OverlayContainer.hpp"

#include "widgets/labels/ColorLabel.hpp"
#include "widgets/labels/TextLabel.hpp"
#include "widgets/labels/ImageLabel.hpp"
#include "widgets/labels/GreyscaleGlass.hpp"
#include "widgets/labels/BlurGlass.hpp"

#include "widgets/TextInput.hpp"
#include "widgets/Slider.hpp"
#include "widgets/Window.hpp"
#include "widgets/MouseProxy.hpp"
#include "widgets/NinePatch.hpp"
#include "widgets/NinePatchButton.hpp"
#include "widgets/TextField.hpp"
#include "widgets/TreeView.hpp"
#include "widgets/CheckBox.hpp"
#include "widgets/List.hpp"
#include "widgets/ResizeProxy.hpp"
#include "widgets/DropDownSelector.hpp"

#include "App.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->AddWidget<Widget>("Widget");
	this->AddWidget<Container>("Container");
	this->AddWidget<HorizontalContainer>("HorizontalContainer");
	this->AddWidget<VerticalContainer>("VerticalContainer");
	this->AddWidget<FrameContainer>("FrameContainer");
	this->AddWidget<TextLabel>("TextLabel");
	this->AddWidget<TextInput>("TextInput");
	this->AddWidget<VerticalSlider>("VerticalSlider");
	this->AddWidget<HorizontalSlider>("HorizontalSlider");
	this->AddWidget<ImageLabel>("ImageLabel");
	this->AddWidget<Window>("Window");
	this->AddWidget<MouseProxy>("MouseProxy");
	this->AddWidget<NinePatch>("NinePatch");
	this->AddWidget<NinePatchButton>("NinePatchButton");
	this->AddWidget<ColorLabel>("ColorLabel");
	this->AddWidget<TextField>("TextField");
	this->AddWidget<CheckBox>("CheckBox");
	this->AddWidget<GreyscaleGlass>("GreyscaleGlass");
	this->AddWidget<BlurGlass>("BlurGlass");
	this->AddWidget<ScrollContainer>("ScrollContainer");
	this->AddWidget<TableContainer>("TableContainer");
	this->AddWidget<TableRow>("TableRow");
	this->AddWidget<HorizontalList>("HorizontalList");
	this->AddWidget<VerticalList>("VerticalList");
	this->AddWidget<KeyProxyContainer>("KeyProxyContainer");
	this->AddWidget<BlackHoleContainer>("BlackHoleContainer");
	this->AddWidget<OverlayContainer>("OverlayContainer");
	this->AddWidget<TreeView>("TreeView");
	this->AddWidget<ResizeProxy>("ResizeProxy");
	this->AddWidget<DropDownSelector>("DropDownSelector");
}



void Inflater::AddWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory){
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



bool Inflater::RemoveWidget(const std::string& widgetName)noexcept{
	if(this->widgetFactories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



std::shared_ptr<morda::Widget> Inflater::Inflate(papki::File& fi) {
	std::unique_ptr<stob::Node> root = this->Load(fi);
	ASSERT(root)

	return std::move(this->Inflate(*root));
}


namespace{

const char* D_Templates = "templates";
const char* D_Defs = "defs";



std::unique_ptr<stob::Node> MergeGUIChain(const stob::Node* from, std::unique_ptr<stob::Node> to){
	if(!to){
		if(!from){
			return nullptr;
		}
		return from->cloneChain();
	}
	
	std::unique_ptr<stob::Node> children; //children will be stored in reverse order
	
	for(auto s = from; s; s = s->next()){
		if(!s->isProperty()){
			auto c = s->clone();
			c->setNext(std::move(children));
			children = std::move(c);
			continue;
		}
		
		if(!s->child() || *s == "@"){ //@ means reference to a variable
			//No children means that it is a property value, stop further processing of this chain.
			
			//Check that it is the only node in the chain
			if(s != from || s->next()){
				throw Inflater::Exc("malformed gui script: property with several values encountered");
			}
			return to;
		}
		
		auto d = to->thisOrNext(s->value()).node();
		if(!d){
			to->insertNext(s->clone());
			continue;
		}
		
		if(!d->child()){
			continue;//no children means that the property is removed in derived template
		}
		
		d->setChildren(MergeGUIChain(s->child(), d->removeChildren()));
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

std::shared_ptr<morda::Widget> Inflater::Inflate(const stob::Node& chain){
	if(!App::inst().ThisIsUIThread()){
		throw Inflater::Exc("Inflate called not from UI thread");
	}
	
	const stob::Node* n = &chain;
	for(; n && n->isProperty(); n = n->next()){
		if(*n == D_Templates){
			if(n->child()){
				this->PushTemplates(n->child()->cloneChain());
			}
		}else if(*n == D_Defs){
			if(n->child()){
				this->PushVariables(*n->child());
			}
		}else{
			throw Exc("Inflater::Inflate(): unknown declaration encountered before first widget");
		}
	}
	
	if(!n){
		return nullptr;
	}
	
	std::unique_ptr<stob::Node> cloned;
	if(auto t = this->FindTemplate(n->value())){
		cloned = utki::makeUnique<stob::Node>(t->value());
		cloned->setChildren(MergeGUIChain(t->child(), n->child() ? n->child()->cloneChain() : nullptr));
		n = cloned.get();
	}
	
	
	auto i = this->widgetFactories.find(n->value());

	if(i == this->widgetFactories.end()){
		TRACE(<< "Inflater::Inflate(): n->value() = " << n->value() << std::endl)
		throw Exc("Failed to inflate, no matching factory found for requested widget name");
	}

	bool needPopTemplates = false;
	bool needPopVariables = false;
	utki::ScopeExit scopeExit([this, &needPopTemplates, &needPopVariables](){
		if(needPopTemplates){
			this->PopTemplates();
		}
		if(needPopVariables){
			this->PopVariables();
		}
	});
	
	if(auto t = n->child(D_Templates).node()){
		if(auto c = t->child()){
			this->PushTemplates(c->cloneChain());
			needPopTemplates = true;
		}
	}
	if(auto v = n->child(D_Defs).node()){
		if(v->child()){
			this->PushVariables(*v->child());
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
		
		this->SubstituteVariables(cloned.get());
		
		return i->second->Create(cloned.get());
	}
}



std::unique_ptr<stob::Node> Inflater::Load(papki::File& fi){
	std::unique_ptr<stob::Node> ret = stob::load(fi);
	
	ret = std::move(std::get<0>(resolveIncludes(fi, std::move(ret))));

	return ret;
}

void Inflater::PushTemplates(std::unique_ptr<stob::Node> chain){
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
		if(auto s = this->FindTemplate(i->second->value())){
			i->second->setValue(s->value());
			ASSERT(s->child())
			i->second->setChildren(MergeGUIChain(s->child(), i->second->removeChildren()));
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



void Inflater::PopTemplates(){
	ASSERT(this->templates.size() != 0)
	this->templates.pop_front();
}



const stob::Node* Inflater::FindTemplate(const std::string& name)const{
	for(auto& i : this->templates){
		auto r = i.find(name);
		if(r != i.end()){
			return r->second.get();
		}
	}
//	TRACE(<< "Inflater::FindTemplate(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



const std::string* Inflater::FindVariable(const std::string& name)const{
	for(auto& i : this->variables){
		auto r = i.find(name);
		if(r != i.end()){
			return &r->second;
		}
	}
	TRACE(<< "Inflater::FindVariable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}



void Inflater::PopVariables(){
	ASSERT(this->variables.size() != 0)
	this->variables.pop_front();
}



void Inflater::PushVariables(const stob::Node& chain){
	decltype(this->variables)::value_type m;
	
	for(auto n = &chain; n; n = n->next()){
		std::string value;
		
		for(auto child = n->child(); child;){
			if(child->next()){
				throw Exc("Inflater::PushVariables(): variable has several values, error");
			}
			
			if(*child == "@" && child->child()){
				auto r = child->child();

				if(r->next()){
					throw Exc("Inflater::PushVariables(): variable reference has several values, error");
				}
				
				if(auto var = this->FindVariable(r->value())){
					value = *var;
				}else{
					throw Exc("Inflater::PushVariables(): variable reference could not be resolved, error");
				}
				
				break;
			}
			
			if(child->child()){
				throw Exc("Inflater::PushVariables(): variable value has children, error");
			}
			
			value = child->value();
			break;
		}
		
		if(!m.insert(
				std::make_pair(n->value(),std::move(value))
			).second)
		{
			throw Exc("Inflater::PushVariables(): failed to add variable, variable with same name is already defined in this variables block");
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



void Inflater::SubstituteVariables(stob::Node* to)const{
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
		
		if(auto var = this->FindVariable(to->child()->value())){
			to->setValue(var->c_str());
			to->removeChildren();
		}
		
		return;
	}
		
	for(; to; to = to->next()){
		if(to->isProperty() && to->child()){
			this->SubstituteVariables(to->child());
		}
	}
}
