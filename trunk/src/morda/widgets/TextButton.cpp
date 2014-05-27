#include "TextButton.hpp"
#include "../App.hpp"



using namespace morda;



void TextButton::ApplyDescription(const stob::Node& description){
	//apply button specific properties
}



//override
void TextButton::Render(const morda::Matr4f& matrix)const{
	this->Button::Render(matrix);
	this->Label::Render(matrix);
}



//override
void TextButton::OnResize(){
	this->Label::OnResize();
}


