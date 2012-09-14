#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void Button::ApplyDescription(const stob::Node& description){
	this->Resize(this->label->Rect().d);
	
	//apply button specific properties
}



//override
void Button::Render(const morda::Matr4f& matrix)const{
	this->AbstractButton::Render(matrix);
	this->label->Render(matrix);
}



void Button::SetText(const std::string& text){
	this->label->SetText(text);
	this->Resize(this->label->Rect().d);
}
