#include "AbstractButton.hpp"
#include "../App.hpp"



using namespace morda;



void AbstractButton::ApplyProperties(const stob::Node* properties){
	//apply abstract button specific properties
}



//override
void AbstractButton::Render(const tride::Matr4f& matrix)const{
	tride::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	
	SimpleSingleColoringShader& s = App::Inst().shaders.simpleSingleColoring;
	s.Bind();
	if(this->isPressed){
		s.SetColor(tride::Vec3f(1, 1, 1));
	}else{
		s.SetColor(tride::Vec3f(0.5, 0.5, 0.5));
	}
	s.SetMatrix(matr);
	s.DrawQuad01();
}



//override
bool AbstractButton::OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Button::OnMouseButtonDown(): enter, button = " << button << ", pos = " << pos << std::endl)
	if(button != LEFT){
		return false;
	}

	this->isPressed = true;

	return true;
}



//override
bool AbstractButton::OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Button::OnMouseButtonUp(): enter, button = " << button << ", pos = " << pos << std::endl)
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
void AbstractButton::OnMouseOut(){
//	TRACE(<< "Button::OnMouseOut(): enter" << std::endl)
	this->isPressed = false;
}
