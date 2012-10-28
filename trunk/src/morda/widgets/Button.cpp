#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void Button::ApplyDescription(const stob::Node& description){
	//apply button specific properties
}



//override
void Button::Render(const morda::Matr4f& matrix)const{
	this->AbstractButton::Render(matrix);
	this->Label::Render(matrix);
}



//override
void Button::OnResize(){
	this->Label::OnResize();
}


