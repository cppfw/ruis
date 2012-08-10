#include "Inflater.hpp"
#include "Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"



using namespace morda;



namespace{

class WidgetFactory : public Inflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Widget")
		
		//TODO:
	}
	
	inline static ting::Ptr<WidgetFactory> New(){
		return ting::Ptr<WidgetFactory>(new WidgetFactory());
	}
};

class ContainerFactory : public Inflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Container")
		
		//TODO:
	}
	
	inline static ting::Ptr<ContainerFactory> New(){
		return ting::Ptr<ContainerFactory>(new ContainerFactory());
	}
};

class LabelFactory : public Inflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Label")
		
		//TODO:
	}
	
	inline static ting::Ptr<LabelFactory> New(){
		return ting::Ptr<LabelFactory>(new LabelFactory());
	}
};

class ButtonFactory : public Inflater::Factory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
		ASSERT(node->Value() == "Button")
		
		//TODO:
	}
	
	inline static ting::Ptr<ButtonFactory> New(){
		return ting::Ptr<ButtonFactory>(new ButtonFactory());
	}
};

}//~namespace



Inflater::Inflater(){
	this->AddFactory("Widget", WidgetFactory::New());
	this->AddFactory("Container", ContainerFactory::New());
	this->AddFactory("Label", LabelFactory::New());
	this->AddFactory("Button", ButtonFactory::New());
	//TODO: add default factories
}



void Inflater::AddFactory(const std::string& widgetName, ting::Ptr<Inflater::Factory> factory){
	std::pair<T_FactoryMap::iterator, bool> ret = this->factories.insert(std::pair<std::string, ting::Ptr<Inflater::Factory> >(widgetName, factory));
	if(!ret.second){
		throw Inflater::Exc("Failed adding factory, factory with that widget name is already added");
	}
}



bool Inflater::RemoveFactory(const std::string& widgetName)throw(){
	if(this->factories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



ting::Ref<morda::Widget> Inflater::Inflate(ting::fs::File& fi)const{
	ting::Ptr<stob::Node> root = stob::Load(fi);
	root->SetValue("Container");
	
	return this->Inflate(root);
}



ting::Ref<morda::Widget> Inflater::Inflate(ting::Ptr<stob::Node> gui)const{
	//TODO:
	
	return ting::Ref<morda::Widget>();
}
