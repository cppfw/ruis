#include "App.hpp"

#include "resources/ResSTOB.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include <papki/FSFile.hpp>
#include <papki/RootDirFile.hpp>

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


#if M_OS == M_OS_UNIX || M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX
#	include <dlfcn.h>
#endif


using namespace morda;



utki::IntrusiveSingleton<App>::T_Instance App::instance;



void App::initStandardWidgets() {
	
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
			auto fi = morda::App::inst().createResourceFileInterface(s);
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
		auto t = morda::App::inst().resMan.load<ResSTOB>("morda_gui_defs");
		
		if(t->chain()){
			this->inflater.inflate(*t->chain());
		}
		
	}catch(ResourceManager::Exc&){
		//ignore
		TRACE(<< "App::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}



void App::render(){
	//TODO: render only if needed?
	
//	TRACE(<< "App::Render(): invoked" << std::endl)
	if(!this->rootWidget){
		TRACE(<< "App::Render(): root widget is not set" << std::endl)
		return;
	}
	
	Render::clearColor();
	
	if(this->windowParams.buffers.get(WindowParams::Buffer_e::DEPTH)){
		Render::clearDepth();
	}
	if(this->windowParams.buffers.get(WindowParams::Buffer_e::STENCIL)){
		Render::clearStencil();
	}
	
	Render::setCullEnabled(true);
	
	morda::Matr4r m;
	m.identity();
	m.translate(-1, -1);
	m.scale(Vec2r(2.0f).compDivBy(this->curWinRect.d));
	
	ASSERT(this->rootWidget)
	
	if(this->rootWidget->needsRelayout()){
		TRACE(<< "root widget re-layout needed!" << std::endl)
		this->rootWidget->relayoutNeeded = false;
		this->rootWidget->layOut();
	}
	
	this->rootWidget->renderInternal(m);
	
	Render::swapFrameBuffers();
}



void App::updateWindowRect(const morda::Rectr& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	TRACE(<< "App::UpdateWindowRect(): this->curWinRect = " << this->curWinRect << std::endl)
	Render::setViewport(kolme::Recti(
			int(this->curWinRect.p.x),
			int(this->curWinRect.p.y),
			int(this->curWinRect.d.x),
			int(this->curWinRect.d.y)
		));
	
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->resize(this->curWinRect.d);
}



void App::handleMouseMove(const morda::Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}
	
	if(this->rootWidget->isInteractive()){
		this->rootWidget->setHovered(this->rootWidget->rect().overlaps(pos), id);
		this->rootWidget->onMouseMove(this->nativeWindowToRootCoordinates(pos), id);
	}
}



void App::handleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::MouseButton_e button, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->isInteractive()){
		this->rootWidget->setHovered(this->rootWidget->rect().overlaps(pos), pointerID);
		this->rootWidget->onMouseButton(isDown, this->nativeWindowToRootCoordinates(pos), button, pointerID);
	}
}



void App::handleMouseHover(bool isHovered, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->setHovered(isHovered, pointerID);
}

void App::handleKeyEvent(bool isDown, Key_e keyCode){
	//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->focusedWidget.lock()){
		//			TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(isDown, keyCode);
	}else{
		//			TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->onKeyInternal(isDown, keyCode);
		}
	}
}




#if M_OS_NAME != M_OS_NAME_ANDROID && M_OS_NAME != M_OS_NAME_IOS
std::unique_ptr<papki::File> App::createResourceFileInterface(const std::string& path)const{
	return utki::makeUnique<papki::FSFile>(path);
}

void App::showVirtualKeyboard()noexcept{
	TRACE(<< "App::ShowVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



void App::hideVirtualKeyboard()noexcept{
	TRACE(<< "App::HideVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}
#endif




void App::setFocusedWidget(const std::shared_ptr<Widget> w){
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



#if M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX || M_OS == M_OS_UNIX
std::unique_ptr<App> morda::createAppUnix(int argc, const char** argv, const utki::Buf<std::uint8_t> savedState){
	void* libHandle = dlopen(nullptr, RTLD_NOW);
	if(!libHandle){
		throw morda::Exc("dlopen(): failed");
	}

	utki::ScopeExit scopeexit([libHandle](){
		dlclose(libHandle);
	});

	auto factory =
			reinterpret_cast<
					std::unique_ptr<App> (*)(int, const char**, const utki::Buf<std::uint8_t>)
				>(dlsym(libHandle, "_ZN5morda9createAppEiPPKcN4utki3BufIhEE"));
	if(!factory){
		throw morda::Exc("dlsym(): createApp() function not found!");
	}

	return factory(argc, argv, savedState);
}
#endif
