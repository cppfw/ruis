#include "push_button.hpp"

using namespace morda;

bool push_button::on_mouse_button(const mouse_button_event& e){
	//TODO: multi-touch support

	if(e.button != mouse_button::left){
		return false;
	}

	if(e.is_down){
		// NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
		//       some other mouse button is clicked, and then the left one is clicked outside of widget,
		//       then the event about left button down will still be delivered to this widget because it has captured the mouse.
		if(this->overlaps(e.pos)){
			this->set_pressed(true);
		}
	}else{
		if(this->is_pressed()){
			this->set_pressed(false);
	//		TRACE(<< "AbstractButton::OnMouseButton(): emitting signal" << std::endl)
		}
	}

	return true;
}

void push_button::on_hover_change(unsigned pointer_id){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)

	//TODO: check if was hovered by the same pointer

	if(!this->is_hovered(pointer_id)){
		if(this->is_pressed()){
			this->set_pressed(false);
		}
	}
}

void push_button::on_press_change(){
	this->button::on_press_change();

	if(this->currentlyPressed && this->is_hovered()){
		ASSERT(!this->is_pressed())
		this->on_click();
	}

	this->currentlyPressed = this->is_pressed();
}

void push_button::on_click(){
	if (this->click_handler){
		this->click_handler(*this);
	}
}
