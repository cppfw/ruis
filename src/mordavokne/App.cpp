#include "App.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include <papki/FSFile.hpp>
#include <papki/RootDirFile.hpp>


#if M_OS == M_OS_UNIX || M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX
#	include <dlfcn.h>
#endif


using namespace morda;



App::T_Instance App::instance;




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
	
	this->swapFrameBuffers();
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



void App::swapFrameBuffers(){
#if M_OS == M_OS_WINDOWS
	SwapBuffers(morda::App::inst().deviceContext.hdc);
#elif M_OS == M_OS_LINUX
#	if M_OS_NAME == M_OS_NAME_ANDROID
	eglSwapBuffers(morda::App::inst().eglDisplay.d, morda::App::inst().eglSurface.s);
#	else
	glXSwapBuffers(morda::App::inst().xDisplay.d, morda::App::inst().xWindow.w);
#	endif
#elif M_OS == M_OS_MACOSX
#	if M_OS_NAME == M_OS_NAME_IOS
	//ios will swap buffers for us
#	else
	morda::App::inst().macosx_SwapFrameBuffers();
#	endif
#else
#	error "unknown OS"
#endif
}
