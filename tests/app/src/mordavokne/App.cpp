#include "App.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include <papki/FSFile.hpp>
#include <papki/RootDirFile.hpp>


using namespace mordavokne;



App::T_Instance App::instance;




void App::render(){
	//TODO: render only if needed?
	this->gui.renderer().clearFramebuffer();
	
	this->gui.render(this->gui.renderer().initialMatrix);
	
	this->swapFrameBuffers();
}



void App::updateWindowRect(const morda::Rectr& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	TRACE(<< "App::UpdateWindowRect(): this->curWinRect = " << this->curWinRect << std::endl)
	this->gui.renderer().setViewport(kolme::Recti(
			int(this->curWinRect.p.x),
			int(this->curWinRect.p.y),
			int(this->curWinRect.d.x),
			int(this->curWinRect.d.y)
		));
	
	this->gui.setViewportSize(this->curWinRect.d);
	
}



#if M_OS_NAME != M_OS_NAME_ANDROID && M_OS_NAME != M_OS_NAME_IOS
std::unique_ptr<papki::File> App::getResFile(const std::string& path)const{
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



morda::real App::findDotsPerDp(kolme::Vec2ui resolution, kolme::Vec2ui screenSizeMm){
	
	//NOTE: for ordinary desktop displays the PT size should be equal to 1 pixel.
	//For high density displays it should be more than one pixel, depending on display ppi.
	//For hand held devices the size of PT should be determined from physical screen size and pixel resolution.

#if M_OS_NAME == M_OS_NAME_IOS
	return morda::real(1);//TODO:
#else
	unsigned xIndex;
	if(resolution.x > resolution.y){
		xIndex = 0;
	}else{
		xIndex = 1;
	}
	
	if(screenSizeMm[xIndex] < 300){
		return resolution[xIndex] / morda::real(700);
	}else if(screenSizeMm[xIndex] < 150) {
        return resolution[xIndex] / morda::real(200);
    }
	
	return morda::real(1);
#endif
}
