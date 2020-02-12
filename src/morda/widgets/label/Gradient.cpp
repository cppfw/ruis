#include "Gradient.hpp"

#include "../../util/util.hpp"

#include "../../context.hpp"

//TODO: remove
#include "../../gui.hpp"

using namespace morda;


Gradient::Gradient(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "gradient"){
			this->gradient = this->context->loader.load<ResGradient>(get_property_value(p).to_string());
		}
	}
}


void Gradient::render(const Matr4r& matrix)const{
	applySimpleAlphaBlending(*this->context->renderer);
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	if(this->gradient){
//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		this->gradient->render(matr);
	}
}
