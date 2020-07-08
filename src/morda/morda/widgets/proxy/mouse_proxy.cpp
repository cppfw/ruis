#include "mouse_proxy.hpp"

using namespace morda;

mouse_proxy::mouse_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{}

bool mouse_proxy::on_mouse_button(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id){
	if(this->mouse_button_handler){
		return this->mouse_button_handler(*this, is_down, pos, button, pointer_id);
	}
	return false;
}

bool mouse_proxy::on_mouse_move(const morda::vector2& pos, unsigned pointer_id){
	if(this->mouse_move_handler){
		return this->mouse_move_handler(*this, pos, pointer_id);
	}
	return false;
}

void mouse_proxy::on_hover_changed(unsigned pointer_id){
	if(this->hover_change_handler){
		this->hover_change_handler(*this, pointer_id);
	}
}
