#include "Morda.hpp"

#include "App.hpp"//TODO: remove

#include "resources/ResSTOB.hpp"

#include "widgets/slider/Slider.hpp"

#include "widgets/button/SimpleButton.hpp"
#include "widgets/button/CheckBox.hpp"
#include "widgets/button/RadioButton.hpp"
#include "widgets/button/ChoiceGroup.hpp"

#include "widgets/label/ColorLabel.hpp"
#include "widgets/label/TextLabel.hpp"
#include "widgets/label/GreyscaleGlass.hpp"
#include "widgets/label/BlurGlass.hpp"

#include "widgets/TextField.hpp"
#include "widgets/List.hpp"
#include "widgets/TreeView.hpp"
#include "widgets/DropDownSelector.hpp"
#include "widgets/Window.hpp"
#include "widgets/MouseCursor.hpp"

using namespace morda;


Morda::T_Instance Morda::instance;



void Morda::initStandardWidgets() {
	
	//mount default resource pack
	
	std::vector<std::string> paths;

#if M_OS == M_OS_WINDOWS
#	ifdef DEBUG
	paths.push_back("../../morda_res/");
#	else
	paths.push_back("morda_res/");
#	endif
#elif M_OS_NAME == M_OS_NAME_IOS || M_OS_NAME == M_OS_NAME_ANDROID
	paths.push_back("morda_res/");
#else //linux or macosx
#	ifdef DEBUG
	paths.push_back("../../morda_res/");
#	else
	
	unsigned soname =
#		include "../soname.txt"
	;
	
	{
		std::stringstream ss;
		ss << "/usr/local/share/morda/res" << soname << "/";
		paths.push_back(ss.str());
	}
	{
		std::stringstream ss;
		ss << "/usr/share/morda/res" << soname << "/";
		paths.push_back(ss.str());
	}
#	endif
#endif

	bool mounted = false;
	for(const auto& s : paths){
		try{
//			TRACE(<< "s = " << s << std::endl)
			auto fi = morda::App::inst().createResourceFileInterface(s);//TODO: remove App
			ASSERT(fi)
//			TRACE(<< "fi->path() = " << fi->path() << std::endl)
			this->resMan.mountResPack(*fi);
		}catch(papki::Exc&){
			continue;
		}
		mounted = true;
		break;
	}

	if(!mounted){
		throw morda::Exc("App::initStandardWidgets(): could not mount default resource pack");
	}
	
	//add standard widgets to inflater
	
	this->inflater.addWidget<TextLabel>("TextLabel");
	this->inflater.addWidget<TextInput>("TextInput");
	this->inflater.addWidget<VerticalSlider>("VerticalSlider");
	this->inflater.addWidget<HorizontalSlider>("HorizontalSlider");
	this->inflater.addWidget<ImageLabel>("ImageLabel");
	this->inflater.addWidget<Window>("Window");
	this->inflater.addWidget<NinePatch>("NinePatch");
	this->inflater.addWidget<SimpleButton>("SimpleButton");
	this->inflater.addWidget<ColorLabel>("ColorLabel");
	this->inflater.addWidget<TextField>("TextField");
	this->inflater.addWidget<CheckBox>("CheckBox");
	this->inflater.addWidget<GreyscaleGlass>("GreyscaleGlass");
	this->inflater.addWidget<BlurGlass>("BlurGlass");
	this->inflater.addWidget<HorizontalList>("HorizontalList");
	this->inflater.addWidget<VerticalList>("VerticalList");
	this->inflater.addWidget<TreeView>("TreeView");
	this->inflater.addWidget<DropDownSelector>("DropDownSelector");
	this->inflater.addWidget<RadioButton>("RadioButton");
	this->inflater.addWidget<ChoiceGroup>("ChoiceGroup");
	this->inflater.addWidget<MouseCursor>("MouseCursor");
	
	
	try{
		auto t = morda::Morda::inst().resMan.load<ResSTOB>("morda_gui_defs");
		
		if(t->chain()){
			this->inflater.inflate(*t->chain());
		}
		
	}catch(ResourceManager::Exc&){
		//ignore
		TRACE(<< "App::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}

