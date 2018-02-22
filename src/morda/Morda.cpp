#include "Morda.hpp"

#include <utki/config.hpp>

#include "res/ResSTOB.hpp"

#include "widgets/slider/Slider.hpp"

#include "widgets/button/NinePatchPushButton.hpp"
#include "widgets/button/CheckBox.hpp"
#include "widgets/button/RadioButton.hpp"
#include "widgets/button/ChoiceGroup.hpp"
#include "widgets/button/ImageToggle.hpp"
#include "widgets/button/ImagePushButton.hpp"
#include "widgets/button/Tab.hpp"
#include "widgets/button/Tabs.hpp"

#include "widgets/label/Color.hpp"
#include "widgets/label/Text.hpp"
#include "widgets/label/Gradient.hpp"

#include "widgets/input/TextField.hpp"
#include "widgets/group/TreeView.hpp"
#include "widgets/button/DropDownSelector.hpp"
#include "widgets/group/Window.hpp"
#include "widgets/label/MouseCursor.hpp"
#include "widgets/group/CollapseArea.hpp"

using namespace morda;


Morda::T_Instance Morda::instance;


Morda::Morda(
		std::shared_ptr<morda::Renderer> r,
		real dotsPerInch,
		real dotsPerPt,
		std::function<void(std::function<void()>&&)>&& postToUiThreadFunction
	) :
		renderer_v(std::move(r)),
		postToUiThread_v(std::move(postToUiThreadFunction)),
		units(dotsPerInch, dotsPerPt)
{
	if(!this->renderer_v){
		throw morda::Exc("no Renderer provided to Morda constructor");
	}
	if(!this->postToUiThread_v){
		throw morda::Exc("no post to UI thread function provided");
	}
}




void Morda::initStandardWidgets(papki::File& fi) {
	
	//mount default resource pack
	
	std::vector<std::string> paths;

	if(fi.path().length() != 0){
		paths.push_back(fi.path());
	}
	
	paths.push_back("morda_res/");
		
#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || \
	(M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
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
#endif

	bool mounted = false;
	for(const auto& s : paths){
		try{
//			TRACE(<< "s = " << s << std::endl)
			fi.setPath(s);
//			TRACE(<< "fi.path() = " << fi.path() << std::endl)
			this->resMan.mountResPack(fi);
		}catch(papki::Exc&){
//			TRACE(<< "could not mount " << s << std::endl)
			continue;
		}
		mounted = true;
		break;
	}

	if(!mounted){
		throw morda::Exc("Morda::initStandardWidgets(): could not mount default resource pack");
	}
	
	//add standard widgets to inflater
	
	this->inflater.registerType<Text>("Text");
	this->inflater.registerType<Color>("Color");
	this->inflater.registerType<Gradient>("Gradient");
	this->inflater.registerType<Image>("Image");
	this->inflater.registerType<VerticalSlider>("VerticalSlider");
	this->inflater.registerType<HorizontalSlider>("HorizontalSlider");
	this->inflater.registerType<Window>("Window");
	this->inflater.registerType<NinePatch>("NinePatch");
	this->inflater.registerType<NinePatchPushButton>("NinePatchPushButton");
	this->inflater.registerType<TextField>("TextField");
	this->inflater.registerType<CheckBox>("CheckBox");
	this->inflater.registerType<TreeView>("TreeView");
	this->inflater.registerType<DropDownSelector>("DropDownSelector");
	this->inflater.registerType<RadioButton>("RadioButton");
	this->inflater.registerType<ChoiceGroup>("ChoiceGroup");
	this->inflater.registerType<MouseCursor>("MouseCursor");
	this->inflater.registerType<CollapseArea>("CollapseArea");
	this->inflater.registerType<ImageToggle>("ImageToggle");
	this->inflater.registerType<ImagePushButton>("ImagePushButton");
	this->inflater.registerType<Tabs>("Tabs");
	this->inflater.registerType<Tab>("Tab");
	
	try{
		auto t = morda::Morda::inst().resMan.load<ResSTOB>("morda_gui_defs");
		
		if(t->chain()){
			this->inflater.inflate(*t->chain());
		}
		
	}catch(ResourceManager::Exc&){
		//ignore
		TRACE(<< "Morda::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}

void Morda::setViewportSize(const morda::Vec2r& size){
	this->viewportSize = size;
	
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->resize(this->viewportSize);
}



void Morda::setRootWidget(const std::shared_ptr<morda::Widget> w){
	this->rootWidget = std::move(w);

	this->rootWidget->moveTo(morda::Vec2r(0));
	this->rootWidget->resize(this->viewportSize);
}

void Morda::render(const Matr4r& matrix)const{
	if(!this->rootWidget){
		TRACE(<< "Morda::render(): root widget is not set" << std::endl)
		return;
	}
	
	morda::Matr4r m(matrix);
	
	//direct y axis down
	m.scale(1, -1);
	
	m.translate(-1, -1);
	m.scale(Vec2r(2).compDivBy(this->viewportSize));
	
	ASSERT(this->rootWidget)
	
	if(this->rootWidget->needsRelayout()){
		TRACE(<< "root widget re-layout needed!" << std::endl)
		this->rootWidget->relayoutNeeded = false;
		this->rootWidget->layOut();
	}
	
	this->rootWidget->renderInternal(m);
}



void Morda::onMouseMove(const Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}
	
	if(this->rootWidget->isInteractive()){
		this->rootWidget->setHovered(this->rootWidget->rect().overlaps(pos), id);
		this->rootWidget->onMouseMove(pos, id);
	}
}



void Morda::onMouseButton(bool isDown, const Vec2r& pos, MouseButton_e button, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->isInteractive()){
		this->rootWidget->setHovered(this->rootWidget->rect().overlaps(pos), pointerID);
		this->rootWidget->onMouseButton(isDown, pos, button, pointerID);
	}
}



void Morda::onMouseHover(bool isHovered, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->setHovered(isHovered, pointerID);
}

void Morda::onKeyEvent(bool isDown, Key_e keyCode){
//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->focusedWidget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(isDown, keyCode);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->onKeyInternal(isDown, keyCode);
		}
	}
}


void Morda::setFocusedWidget(const std::shared_ptr<Widget> w){
	if(auto prev = this->focusedWidget.lock()){
		prev->isFocused_v = false;
		prev->onFocusChanged();
	}
	
	this->focusedWidget = w;
	
	if(w){
		w->isFocused_v = true;
		w->onFocusChanged();
	}
}

void Morda::onCharacterInput(const UnicodeProvider& unicode, Key_e key){
	if(auto w = this->focusedWidget.lock()){
		//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
		if(auto c = dynamic_cast<CharInputWidget*>(w.operator->())){
			c->onCharacterInput(unicode.get(), key);
		}
	}
}

void Morda::postToUiThread(std::function<void()>&& f) {
	ASSERT(this->postToUiThread_v)
	
	this->postToUiThread_v(std::move(f));
}
