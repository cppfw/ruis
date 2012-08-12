#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void Button::ApplyProperties(const stob::Node* properties){
	this->Resize(this->label->Rect().d);
	
	//apply button specific properties
}



//override
void Button::Render(const tride::Matr4f& matrix)const{
	this->AbstractButton::Render(matrix);
	this->label->Render(matrix);
}



void Button::SetText(const std::string& text){
	this->label->SetText(text);
	this->Resize(this->label->Rect().d);
}
