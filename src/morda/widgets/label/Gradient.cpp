#include "Gradient.hpp"

#include "../../util/util.hpp"

#include "../../Morda.hpp"

using namespace morda;


Gradient::Gradient(const puu::forest& desc) :
		widget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "gradient"){
			this->gradient = morda::inst().resMan.load<ResGradient>(get_property_value(p).to_string());
		}
	}
}


void Gradient::render(const Matr4r& matrix)const{
	applySimpleAlphaBlending();
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	if(this->gradient){
//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		this->gradient->render(matr);
	}
}
