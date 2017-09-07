#include "Color.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


Color::Color(const stob::Node* chain) :
		Widget(chain),
		ColorWidget(chain)
{
	if(auto p = getProperty(chain, "gradient")){
		this->gradient = Morda::inst().resMan.load<ResGradient>(p->value());
	}else{
		ASSERT(!this->gradient)
	}
}



void Color::render(const morda::Matr4r& matrix)const{
	applySimpleAlphaBlending();
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	if(this->gradient){
//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		this->gradient->render(matr);
	}else{
		auto& r = morda::inst().renderer();
		r.shader->colorPos->render(matr, this->color(), *r.posQuad01VAO);
	}
}
