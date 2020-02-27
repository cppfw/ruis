#include "PushButton.hpp"


using namespace morda;


bool PushButton::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId){
	//TODO: multi-touch support

//	TRACE(<< "AbstractButton::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	if(button != mouse_button::left){
		return false;
	}

	if(isDown){
		// NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
		//       some other mouse button is clicked, and then the left one is clicked outside of widget,
		//       then the event about left button down will still be delivered to this widget because it has captured the mouse.
		if(this->overlaps(pos)){
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



void PushButton::on_hover_changed(unsigned pointerId){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)

	//TODO: check if was hovered by the same pointer

	if(!this->is_hovered(pointerId)){
		if(this->is_pressed()){
			this->set_pressed(false);
		}
	}
}

void PushButton::on_pressed_changed(){
	this->button::on_pressed_changed();

	if(this->currentlyPressed && this->is_hovered()){
		ASSERT(!this->is_pressed())
		this->onClicked();
	}

	this->currentlyPressed = this->is_pressed();
}

void PushButton::onClicked(){
	if (this->clicked){
		this->clicked(*this);
	}
}
