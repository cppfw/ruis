#include "mouse_proxy.hpp"

using namespace morda;

mouse_proxy::mouse_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{}

bool mouse_proxy::on_mouse_button(const mouse_button_event& e){
	if(this->mouse_button_handler){
		return this->mouse_button_handler(*this, e);
	}
	return false;
}

bool mouse_proxy::on_mouse_move(const mouse_move_event& e){
	// LOG("e.pos = " << e.pos << ", rect() = " << this->rect() << std::endl)
	if(this->mouse_move_handler){
		return this->mouse_move_handler(*this, e);
	}
	return false;
}

void mouse_proxy::on_hover_change(unsigned pointer_id){
	if(this->hover_change_handler){
		this->hover_change_handler(*this, pointer_id);
	}
}
