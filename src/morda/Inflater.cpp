#include "Inflater.hpp"

#include "widgets/Container.hpp"
#include "widgets/ColorLabel.hpp"
#include "widgets/Label.hpp"
#include "widgets/LinearContainer.hpp"
#include "widgets/FrameContainer.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/Slider.hpp"
#include "widgets/ImageLabel.hpp"
#include "widgets/Window.hpp"
#include "widgets/MouseProxy.hpp"
#include "widgets/NinePatch.hpp"
#include "widgets/PushButton.hpp"
#include "widgets/TextField.hpp"

#include "App.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->AddWidget<Widget>("Widget");
	this->AddWidget<Container>("Container");
	this->AddWidget<LinearContainer>("LinearContainer");
	this->AddWidget<FrameContainer>("FrameContainer");
	this->AddWidget<Label>("Label");
	this->AddWidget<TextInput>("TextInput");
	this->AddWidget<Slider>("Slider");
	this->AddWidget<ImageLabel>("ImageLabel");
	this->AddWidget<Window>("Window");
	this->AddWidget<MouseProxy>("MouseProxy");
	this->AddWidget<NinePatch>("NinePatch");
	this->AddWidget<PushButton>("PushButton");
	this->AddWidget<ColorLabel>("ColorLabel");
	this->AddWidget<TextField>("TextField");
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



std::shared_ptr<morda::Widget> Inflater::Inflate(ting::fs::File& fi) const{
	std::unique_ptr<stob::Node> root = this->Load(fi);
	ASSERT(root)

	return std::move(this->Inflate(*root));
}



std::shared_ptr<morda::Widget> Inflater::Inflate(const stob::Node& gui)const{
	if(!App::Inst().ThisIsUIThread()){
		throw Inflater::Exc("Inflate called not from UI thread");
	}
	
	const stob::Node* n = &gui;
	for(; ; n = n->Next()){
		if(!n){
			return 0;
		}
		
		if(!n->IsProperty()){
			break;
		}
		
		if(*n == "class"){
			n = n->Next();
			if(!n){
				TRACE(<< "Inflater::Inflate(): unexpected end of document after class keyword"<< std::endl)
				return 0;
			}
			
			if(n->IsProperty()){
				TRACE(<< "Inflater::Inflate(): class keyword is not followed by a class name (Should start with capital letter)"<< std::endl)
				return 0;
			}
			
			//TODO: save class name
			
			n = n->Next();
			if(!n){
				TRACE(<< "Inflater::Inflate(): unexpected end of document after class name"<< std::endl)
				return 0;
			}
			
			//TODO: search for inherited class existence
			
			//TODO: add new class
		}
	}
	
	T_FactoryMap::const_iterator i = this->widgetFactories.find(n->Value());

	if(i == this->widgetFactories.end()){
		TRACE(<< "Inflater::Inflate(): n->Value() = " << n->Value() << std::endl)
		throw Inflater::Exc("Failed to inflate, no matching factory found for requested widget name");
	}

	return std::move(i->second->Create(n->Child()));
}



std::unique_ptr<stob::Node> Inflater::Load(ting::fs::File& fi){
	std::unique_ptr<stob::Node> ret = stob::Load(fi);
	
	ret = std::move(std::get<0>(ResolveIncludes(fi, std::move(ret))));

	return ret;
}
