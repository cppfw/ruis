#include "MouseProxy.hpp"


using namespace morda;


MouseProxy::MouseProxy(const stob::Node* chain) :
		Widget(chain)
{}



bool MouseProxy::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(this->onMouseButton){
		return this->onMouseButton(*this, isDown, pos, button, pointerId);
	}
	return false;
}



bool MouseProxy::OnMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	if(this->onMouseMove){
		return this->onMouseMove(*this, pos, pointerId);
	}
	return false;
}



void MouseProxy::OnHoverChanged(){
	if(this->onHoverChanged){
		this->onHoverChanged(*this);
	}
}
