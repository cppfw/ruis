#include "push_button.hpp"

using namespace morda;

bool push_button::on_mouse_button(const mouse_button_event& e){
	if(e.button != mouse_button::left){
		return false;
	}

	if(e.is_down){
		if(!this->is_pressed()){
			// NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
			//       some other mouse button is clicked, and then the left one is clicked outside of widget,
			//       then the event about left button down will still be delivered to this widget because it has captured the mouse.
			if(this->overlaps(e.pos)){
				this->set_pressed(true);
				this->pointer_id = e.pointer_id;
			}
		}
	}else{
		if(this->is_pressed()){
			if(this->pointer_id == e.pointer_id){ // check that released the same pointer which has pressed the button before
				this->set_pressed(false);
			}
		}
	}

	return true;
}

void push_button::on_hover_change(unsigned pointer_id){
//	TRACE(<< "push_button::on_hover_change(): enter" << std::endl)

	if(!this->is_hovered(pointer_id)){
		if(this->is_pressed() && this->pointer_id == pointer_id){
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
