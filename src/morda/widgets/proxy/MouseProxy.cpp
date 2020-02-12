#include "MouseProxy.hpp"


using namespace morda;


MouseProxy::MouseProxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{}



bool MouseProxy::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(this->mouseButton){
		return this->mouseButton(*this, isDown, pos, button, pointerId);
	}
	return false;
}



bool MouseProxy::on_mouse_move(const morda::Vec2r& pos, unsigned pointerId){
	if(this->mouseMove){
		return this->mouseMove(*this, pos, pointerId);
	}
	return false;
}



void MouseProxy::on_hover_changed(unsigned pointerID){
	if(this->hoverChanged){
		this->hoverChanged(*this, pointerID);
	}
}
