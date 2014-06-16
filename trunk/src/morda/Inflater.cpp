#include "Inflater.hpp"

#include "widgets/Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/LinearContainer.hpp"
#include "widgets/FrameContainer.hpp"
#include "widgets/TextButton.hpp"
#include "widgets/Slider.hpp"



using namespace morda;



Inflater::Inflater(){
	this->AddWidget<Widget>("Widget");
	this->AddWidget<Container>("Container");
	this->AddWidget<LinearContainer>("LinearContainer");
	this->AddWidget<FrameContainer>("FrameContainer");
	this->AddWidget<Label>("Label");
	this->AddWidget<TextButton>("TextButton");
	this->AddWidget<Slider>("Slider");
}



void Inflater::AddWidgetFactory(const std::string& widgetName, ting::Ptr<WidgetFactory> factory){
	std::pair<T_FactoryMap::iterator, bool> ret = this->widgetFactories.insert(
			std::pair<std::string, ting::Ptr<Inflater::WidgetFactory> >(
					widgetName,
					factory
				)
		);
	if(!ret.second){
		throw Inflater::Exc("Failed registering widget type, widget type with given name is already added");
	}
}



bool Inflater::RemoveWidget(const std::string& widgetName)throw(){
	if(this->widgetFactories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



ting::Ref<morda::Widget> Inflater::Inflate(ting::fs::File& fi) const{
	ting::Ptr<stob::Node> root = stob::Load(fi);
	ASSERT(root)
	
	stob::Node* first = root->Child();
	if(!first){
		return 0;
	}
	
	return this->Inflate(*first);
}



ting::Ref<morda::Widget> Inflater::Inflate(const stob::Node& gui)const{
	T_FactoryMap::const_iterator i = this->widgetFactories.find(gui.Value());
	
	if(i == this->widgetFactories.end()){
		throw Inflater::Exc("Failed to inflate, no matching factory found for requested widget name");
	}
	
	return i->second->Create(gui);
}
