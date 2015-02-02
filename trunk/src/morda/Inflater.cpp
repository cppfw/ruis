#include "Inflater.hpp"

#include "widgets/containers/Container.hpp"
#include "widgets/containers/LinearContainer.hpp"
#include "widgets/containers/FrameContainer.hpp"
#include "widgets/containers/ScrollContainer.hpp"
#include "widgets/containers/DimContainer.hpp"

#include "widgets/labels/ColorLabel.hpp"
#include "widgets/labels/Label.hpp"
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
#include "widgets/CheckBox.hpp"

#include "App.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->AddWidget<Widget>("Widget");
	this->AddWidget<Container>("Container");
	this->AddWidget<HorizontalContainer>("HorizontalContainer");
	this->AddWidget<VerticalContainer>("VerticalContainer");
	this->AddWidget<FrameContainer>("FrameContainer");
	this->AddWidget<Label>("Label");
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
	this->AddWidget<DimContainer>("DimContainer");
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



bool Inflater::RemoveWidget(const std::string& widgetName)NOEXCEPT{
	if(this->widgetFactories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



std::shared_ptr<morda::Widget> Inflater::Inflate(ting::fs::File& fi) {
	std::unique_ptr<stob::Node> root = this->Load(fi);
	ASSERT(root)

	return std::move(this->Inflate(*root));
}


namespace{

const char* D_Templates = "templates";
const char* D_Vars = "vars";



std::unique_ptr<stob::Node> MergeGUIChain(const stob::Node* from, std::unique_ptr<stob::Node> to){
	if(!to){
		if(!from){
			return nullptr;
		}
		return from->CloneChain();
	}
	
	std::unique_ptr<stob::Node> children; //children will be stored in reverse order
	
	for(auto s = from; s; s = s->Next()){
		if(!s->IsProperty()){
			auto c = s->Clone();
			c->SetNext(std::move(children));
			children = std::move(c);
			continue;
		}
		
		if(!s->Child() || *s == "@"){ //@ means reference to a variable
			//No children means that it is a property value, stop further processing of this chain.
			
			//Check that it is the only node in the chain
			if(s != from || s->Next()){
				throw Inflater::Exc("malformed gui script: property with several values encountered");
			}
			return to;
		}
		
		auto d = to->ThisOrNext(s->Value()).node();
		if(!d){
			to->InsertNext(s->Clone());
			continue;
		}
		
		if(!d->Child()){
			continue;//no children means that the property is removed in derived template
		}
		
		d->SetChildren(MergeGUIChain(s->Child(), d->RemoveChildren()));
	}
	
	//add children in reverse order again, so it will be in normal order in the end
	for(; children;){
		auto c = std::move(children);
		children = c->ChopNext();
		
		c->SetNext(std::move(to));
		to = std::move(c);
	}
	
	return to;
}

}

std::shared_ptr<morda::Widget> Inflater::Inflate(const stob::Node& chain){
	if(!App::Inst().ThisIsUIThread()){
		throw Inflater::Exc("Inflate called not from UI thread");
	}
	
	const stob::Node* n = &chain;
	for(; n && n->IsProperty(); n = n->Next()){
		if(*n == D_Templates){
			if(n->Child()){
				this->PushTemplates(n->Child()->CloneChain());
			}
		}else if(*n == D_Vars){
			if(n->Child()){
				this->PushVariables(*n->Child());
			}
		}else{
			throw Exc("Inflater::Inflate(): unknown declaration encountered before first widget");
		}
	}
	
	if(!n){
		return nullptr;
	}
	
	std::unique_ptr<stob::Node> cloned;
	if(auto t = this->FindTemplate(n->Value())){
		cloned = stob::Node::New(t->Value());
		cloned->SetChildren(MergeGUIChain(t->Child(), n->Child() ? n->Child()->CloneChain() : nullptr));
		n = cloned.get();
	}
	
	
	T_FactoryMap::const_iterator i = this->widgetFactories.find(n->Value());

	if(i == this->widgetFactories.end()){
		TRACE(<< "Inflater::Inflate(): n->Value() = " << n->Value() << std::endl)
		throw Exc("Failed to inflate, no matching factory found for requested widget name");
	}

	bool needPopTemplates = false;
	bool needPopVariables = false;
	ting::util::ScopeExit scopeExit([this, &needPopTemplates, &needPopVariables](){
		if(needPopTemplates){
			this->PopTemplates();
		}
		if(needPopVariables){
			this->PopVariables();
		}
	});
	
	if(auto t = n->Child(D_Templates).node()){
		if(auto c = t->Child()){
			this->PushTemplates(c->CloneChain());
			needPopTemplates = true;
		}
	}
	if(auto v = n->Child(D_Vars).node()){
		if(v->Child()){
			this->PushVariables(*v->Child());
			needPopVariables = true;
		}
	}
	
	{
		if(cloned){
			cloned = cloned->RemoveChildren();
		}else{
			if(n->Child()){
				cloned = n->Child()->CloneChain();
			}
		}
		
		this->SubstituteVariables(cloned.get());
		
		return i->second->Create(cloned.get());
	}
}



std::unique_ptr<stob::Node> Inflater::Load(ting::fs::File& fi){
	std::unique_ptr<stob::Node> ret = stob::Load(fi);
	
	ret = std::move(std::get<0>(ResolveIncludes(fi, std::move(ret))));

	return ret;
}



void Inflater::PushTemplates(std::unique_ptr<stob::Node> chain){
	decltype(this->templates)::value_type m;
	
	for(; chain; chain = chain->ChopNext()){
		if(chain->IsProperty()){
			throw Exc("Inflater::PushTemplates(): template name does not start with capital latin letter, error.");
		}
		
		if(chain->Child()){
			throw Exc("Inflater::PushTemplates(): template name has children, error.");
		}
		
		if(!chain->Next()){
			throw Exc("Inflater::PushTemplates(): template name is not followed by template definition, error.");
		}
		
		if(!m.insert(std::make_pair(chain->Value(), chain->RemoveNext())).second){
			throw Exc("Inflater::PushTemplates(): template name is already defined in given templates chain, error.");
		}
	}
	
	for(auto i = m.begin(); i != m.end(); ++i){
		if(auto s = this->FindTemplate(i->second->Value())){
			i->second->SetValue(s->Value());
			ASSERT(s->Child())
			i->second->SetChildren(MergeGUIChain(s->Child(), i->second->RemoveChildren()));
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
	
	for(auto n = &chain; n; n = n->Next()){
		std::string value;
		
		for(auto child = n->Child(); child;){
			if(child->Next()){
				throw Exc("Inflater::PushVariables(): variable has several values, error");
			}
			
			if(*child == "@" && child->Child()){
				auto r = child->Child();

				if(r->Next()){
					throw Exc("Inflater::PushVariables(): variable reference has several values, error");
				}
				
				if(auto var = this->FindVariable(r->Value())){
					value = *var;
				}else{
					throw Exc("Inflater::PushVariables(): variable reference could not be resolved, error");
				}
				
				break;
			}
			
			if(child->Child()){
				throw Exc("Inflater::PushVariables(): variable value has children, error");
			}
			
			value = child->Value();
			break;
		}
		
		if(!m.insert(
				std::make_pair(n->Value(),std::move(value))
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
		if(to->Next()){
			throw Exc("Inflater::SubstituteVariables(): error: property value holds something else besides reference to a variable");
		}
		
		if(!to->Child()){
			throw Exc("Inflater::SubstituteVariables(): error: reference to a variable holds no variable name");
		}
		
		if(to->Child()->Next()){
			throw Exc("Inflater::SubstituteVariables(): error: reference to variable holds more than one variable name");
		}
		
		if(to->Child()->Child()){
			throw Exc("Inflater::SubstituteVariables(): error: variable name has children");
		}
		
		if(auto var = this->FindVariable(to->Child()->Value())){
			to->SetValue(var->c_str());
			to->RemoveChildren();
		}
		
		return;
	}
		
	for(; to; to = to->Next()){
		if(to->IsProperty() && to->Child()){
			this->SubstituteVariables(to->Child());
		}
	}
}
