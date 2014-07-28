#include "TextButton.hpp"
#include "../App.hpp"



using namespace morda;



//override
void TextButton::Render(const morda::Matr4f& matrix)const{
	this->Button::Render(matrix);
	this->Label::Render(matrix);
}



//override
void TextButton::OnResize(){
	this->Label::OnResize();
}


