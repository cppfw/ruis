#include "GuiInflater.hpp"
#include "Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"



using namespace morda;



namespace{

class WidgetFactory : public GuiInflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Widget")
		return Widget::New(node);
	}
	
	inline static ting::Ptr<WidgetFactory> New(){
		return ting::Ptr<WidgetFactory>(new WidgetFactory());
	}
};

class ContainerFactory : public GuiInflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Container")
		return Container::New(node);
	}
	
	inline static ting::Ptr<ContainerFactory> New(){
		return ting::Ptr<ContainerFactory>(new ContainerFactory());
	}
};

class LabelFactory : public GuiInflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Label")
		return Label::New(node);
	}
	
	inline static ting::Ptr<LabelFactory> New(){
		return ting::Ptr<LabelFactory>(new LabelFactory());
	}
};

class ButtonFactory : public GuiInflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Button")
		return Button::New(node);
	}
	
	inline static ting::Ptr<ButtonFactory> New(){
		return ting::Ptr<ButtonFactory>(new ButtonFactory());
	}
};

}//~namespace



GuiInflater::GuiInflater(){
	this->AddFactory("Widget", WidgetFactory::New());
	this->AddFactory("Container", ContainerFactory::New());
	this->AddFactory("Label", LabelFactory::New());
	this->AddFactory("Button", ButtonFactory::New());
	//TODO: add default factories
}



void GuiInflater::AddFactory(const std::string& widgetName, ting::Ptr<GuiInflater::Factory> factory){
	std::pair<T_FactoryMap::iterator, bool> ret = this->factories.insert(std::pair<std::string, ting::Ptr<GuiInflater::Factory> >(widgetName, factory));
	if(!ret.second){
		throw GuiInflater::Exc("Failed adding factory, factory with that widget name is already added");
	}
}



bool GuiInflater::RemoveFactory(const std::string& widgetName)throw(){
	if(this->factories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



ting::Ref<morda::Widget> GuiInflater::Inflate(ting::fs::File& fi)const{
	ting::Ptr<stob::Node> root = stob::Load(fi);
	root->SetValue("Container");
	
	return this->Inflate(root);
}



ting::Ref<morda::Widget> GuiInflater::Inflate(ting::Ptr<stob::Node> gui)const{
	if(gui.IsNotValid()){
		throw GuiInflater::Exc("Failed to inflate, passed pointer to GUI STOB hierarchy is not valid");
	}
	
	T_FactoryMap::const_iterator i = this->factories.find(gui->Value());
	
	if(i == this->factories.end()){
		throw GuiInflater::Exc("Failed to inflate, no matching factory found for requested widget name");
	}
	
	return i->second->Create(gui);
}
