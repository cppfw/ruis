#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void AbstractButton::ApplyDescription(const stob::Node& properties){
	//apply abstract button specific properties
}



//override
void AbstractButton::Render(const morda::Matr4f& matrix)const{
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	
	SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	if(this->isPressed){
		s.SetColor(morda::Vec3f(1, 1, 1));
	}else{
		s.SetColor(morda::Vec3f(0.5, 0.5, 0.5));
	}
	s.SetMatrix(matr);
	s.DrawQuad01();
}



//override
bool AbstractButton::OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "AbstractButton::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	if(button != LEFT){
		return false;
	}

	if(isDown){
		this->isPressed = true;
	}else{
		if(this->isPressed){
			this->isPressed = false;
	//		TRACE(<< "AbstractButton::OnMouseButton(): emitting signal" << std::endl)
			this->pressed.Emit();
			return true;
		}
	}

	return true;
}



//override
void AbstractButton::OnHoverChanged(){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)
	
	if(!this->IsHovered()){
		this->isPressed = false;
	}
}
