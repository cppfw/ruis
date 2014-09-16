#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void Button::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);
	
	ColorPosShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	if(this->isPressed){
		s.SetColor(morda::Vec3f(1, 1, 1));
	}else{
		s.SetColor(morda::Vec3f(0.5, 0.5, 0.5));
	}
	s.SetMatrix(matr);
	s.Render(s.quad01Fan, Shader::EMode::TRIANGLE_FAN);
}



//override
bool Button::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "AbstractButton::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	if(button != EMouseButton::LEFT){
		return false;
	}

	if(isDown){
		//NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
		//some other mouse button is clicked, and then the left one is clicked outside of widget,
		//then the event about left button down will still be delivered to this widget because it has captured the mouse.
		if(this->Contains(pos)){
			this->isPressed = true;
			this->OnPressedChanged();
		}
	}else{
		if(this->isPressed){
			this->isPressed = false;
			this->OnPressedChanged();
	//		TRACE(<< "AbstractButton::OnMouseButton(): emitting signal" << std::endl)
			if(this->onPressed){
				this->onPressed();
			}
			return true;
		}
	}

	return true;
}



//override
void Button::OnHoverChanged(){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)
	
	if(!this->IsHovered()){
		this->isPressed = false;
		this->OnPressedChanged();
	}
}
