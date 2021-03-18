#include "application.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include <papki/fs_file.hpp>
#include <papki/root_dir.hpp>

using namespace mordavokne;

application::T_Instance application::instance;

void application::render(){
	//TODO: render only if needed?
	this->gui.context->renderer->clear_framebuffer();

	this->gui.render(this->gui.context->renderer->initial_matrix);

	this->swap_frame_buffers();
}

void application::update_window_rect(const morda::rectangle& rect){
	if(this->curWinRect == rect){
		return;
	}

	this->curWinRect = rect;

	LOG("application::update_window_rect(): this->curWinRect = " << this->curWinRect << std::endl)
	this->gui.context->renderer->set_viewport(r4::rectangle<int>(
			int(this->curWinRect.p.x()),
			int(this->curWinRect.p.y()),
			int(this->curWinRect.d.x()),
			int(this->curWinRect.d.y())
		));

	this->gui.set_viewport(this->curWinRect.d);
}

#if M_OS_NAME != M_OS_NAME_ANDROID && M_OS_NAME != M_OS_NAME_IOS
std::unique_ptr<papki::file> application::get_res_file(const std::string& path)const{
	return utki::make_unique<papki::fs_file>(path);
}

void application::show_virtual_keyboard()noexcept{
	LOG("application::show_virtual_keyboard(): invoked" << std::endl)
	// do nothing
}

void application::hide_virtual_keyboard()noexcept{
	LOG("application::hide_virtual_keyboard(): invoked" << std::endl)
	// do nothing
}
#endif

morda::real application::get_pixels_per_dp(r4::vector2<unsigned> resolution, r4::vector2<unsigned> screenSizeMm){

	// NOTE: for ordinary desktop displays the PT size should be equal to 1 pixel.
	// For high density displays it should be more than one pixel, depending on display ppi.
	// For hand held devices the size of PT should be determined from physical screen size and pixel resolution.

#if M_OS_NAME == M_OS_NAME_IOS
	return morda::real(1); // TODO:
#else
	unsigned xIndex;
	if(resolution.x() > resolution.y()){
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
