#include "Gradient.hpp"

#include "../../util/util.hpp"

#include "../../Morda.hpp"

using namespace morda;


Gradient::Gradient(const stob::Node* chain) :
		Widget(chain)
{
	if(auto p = getProperty(chain, "gradient")){
		this->gradient = Morda::inst().resMan.load<ResGradient>(p->value());
	}else{
		ASSERT(!this->gradient)
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
