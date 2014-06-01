#include "Slider.hpp"


using namespace morda;



void Slider::ApplyDescription(const stob::Node& description){
	if(const stob::Node* n = description.GetProperty("vertical")){
		this->isVertical = n->AsBool();
	}
	
	//TODO:
}



void Slider::Render(const morda::Matr4f& matrix)const{
	//TODO:
}
