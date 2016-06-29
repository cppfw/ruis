#include "Button.hpp"

#include "../../App.hpp"

#include "../../util/util.hpp"


using namespace morda;



bool Button::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	//TODO: multitouch support
	
//	TRACE(<< "AbstractButton::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	if(button != EMouseButton::LEFT){
		return false;
	}

	if(isDown){
		//NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
		//some other mouse button is clicked, and then the left one is clicked outside of widget,
		//then the event about left button down will still be delivered to this widget because it has captured the mouse.
		if(this->contains(pos)){
			this->isPressed_v = true;
			this->onPressedChanged();
		}
	}else{
		if(this->isPressed_v){
			this->isPressed_v = false;
			this->onPressedChanged();
	//		TRACE(<< "AbstractButton::OnMouseButton(): emitting signal" << std::endl)
		}
	}

	return true;
}



void Button::onHoverChanged(unsigned pointerId){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)
	
	//TODO: check if was hovered by the same pointer
	
	if(!this->isHovered(pointerId)){
		if(this->isPressed_v){
			this->isPressed_v = false;
			this->onPressedChanged();
		}
	}
}


void PushButton::onPressedChanged() {
	this->Button::onPressedChanged();
	
	if(this->currentlyPressed && this->isHovered()){
		ASSERT(!this->isPressed())
		this->onClicked();
	}
	
	this->currentlyPressed = this->isPressed();
}



ToggleButton::ToggleButton(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = getProperty(chain, "checked")){
		this->isChecked_var = n->asBool();
	}else{
		this->isChecked_var = false;
	}
}

