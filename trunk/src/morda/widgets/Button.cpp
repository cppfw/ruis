#include "Button.hpp"



using namespace morda;



//override
void Button::Render(const tride::Matr4f& matrix)const{
	//TODO:
}



//override
bool Button::OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
	if(button != LEFT){
		return false;
	}

	this->isPressed = true;

	return true;
}



//override
bool Button::OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
	if(button != LEFT){
		return false;
	}
	
	if(!this->Rect().Overlaps(pos)){
		this->isPressed = false;
		return true;
	}
	
	if(this->isPressed){
		this->isPressed = false;
		this->pressed.Emit();
	}
	
	return true;
}



//override
void Button::OnMouseOut(){
	this->isPressed = false;
}
