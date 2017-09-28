#include "Button.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;

Button::Button(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = getProperty(chain, "pressed")){
		this->isPressed_v = n->asBool();
	}else{
		this->isPressed_v = false;
	}
}


void Button::setPressed(bool pressed) {
	if(this->isPressed_v == pressed){
		return;
	}
	this->isPressed_v = pressed;
	this->isPressedChangedNotified = false;
	this->onPressedChanged();
}



void Button::onPressedChanged() {
	if(this->isPressedChangedNotified){
		return;
	}
	this->isPressedChangedNotified = true;
	
	if (this->pressedChanged) {
		this->pressedChanged(*this);
	}
}
