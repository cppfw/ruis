#include "PushButton.hpp"


using namespace morda;


void PushButton::onPressedChanged() {
	this->Button::onPressedChanged();
	
	if(this->currentlyPressed && this->isHovered()){
		ASSERT(!this->isPressed())
		this->onClicked();
	}
	
	this->currentlyPressed = this->isPressed();
}

void PushButton::onClicked() {
	if (this->clicked) {
		this->clicked(*this);
	}
}
