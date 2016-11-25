#include "App.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include <papki/FSFile.hpp>
#include <papki/RootDirFile.hpp>


#if M_OS == M_OS_UNIX || M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX
#	include <dlfcn.h>
#endif


using namespace mordavokne;



App::T_Instance App::instance;




void App::render(){
	//TODO: render only if needed?
	
	this->renderer->clearFramebuffer();

	this->gui.render();
	
	this->swapFrameBuffers();
}



void App::updateWindowRect(const morda::Rectr& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	TRACE(<< "App::UpdateWindowRect(): this->curWinRect = " << this->curWinRect << std::endl)
	morda::inst().renderer().setViewport(kolme::Recti(
			int(this->curWinRect.p.x),
			int(this->curWinRect.p.y),
			int(this->curWinRect.d.x),
			int(this->curWinRect.d.y)
		));
	
	this->gui.setViewportSize(this->curWinRect.d);
	
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




#if M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX || M_OS == M_OS_UNIX
std::unique_ptr<App> mordavokne::createAppUnix(int argc, const char** argv, const utki::Buf<std::uint8_t> savedState){
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
				>(dlsym(libHandle, "_ZN10mordavokne9createAppEiPPKcN4utki3BufIhEE"));
	if(!factory){
		throw morda::Exc("dlsym(): createApp() function not found!");
	}

	return factory(argc, argv, savedState);
}
#endif


#if M_OS != M_OS_MACOSX
void App::swapFrameBuffers(){
#if M_OS == M_OS_WINDOWS
	SwapBuffers(this->deviceContext.hdc);
#elif M_OS == M_OS_LINUX
#	if M_OS_NAME == M_OS_NAME_ANDROID
	eglSwapBuffers(morda::App::inst().eglDisplay.d, morda::App::inst().eglSurface.s);
#	else
	glXSwapBuffers(this->xDisplay.d, this->xWindow.w);
#	endif
#else
#	error "unknown OS"
#endif
}
#endif



App::GLEWWrapper::GLEWWrapper(){
#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
	if(glewInit() != GLEW_OK){
		throw utki::Exc("GLEW initialization failed");
	}
#endif
}
