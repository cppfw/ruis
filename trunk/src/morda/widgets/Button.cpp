#include "Button.hpp"
#include "../App.hpp"



using namespace morda;



void Button::ApplyDescription(const stob::Node& description){
	//apply button specific properties
}



//override
void Button::Render(const morda::Matr4f& matrix)const{
	this->AbstractButton::Render(matrix);
	this->label->Render(matrix);
}



//override
void Button::OnResize(){
	this->label->Resize(this->Rect().d);
}



//override
morda::Vec2f Button::ComputeMinDim()const throw(){
	return this->label->GetMinDim();
}



void Button::SetText(const std::string& text){
	this->label->SetText(text);
	this->RelayoutNeeded();
}
