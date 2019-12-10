#include "MouseProxy.hpp"


using namespace morda;


MouseProxy::MouseProxy(const stob::Node* chain) :
		Widget(chain)
{}



bool MouseProxy::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(this->mouseButton){
		return this->mouseButton(*this, isDown, pos, button, pointerId);
	}
	return false;
}



bool MouseProxy::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	if(this->mouseMove){
		return this->mouseMove(*this, pos, pointerId);
	}
	return false;
}



void MouseProxy::onHoverChanged(unsigned pointerID){
	if(this->hoverChanged){
		this->hoverChanged(*this, pointerID);
	}
}
