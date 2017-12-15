#include "Color.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


Color::Color(const stob::Node* chain) :
		Widget(chain),
		ColorWidget(chain)
{
}



void Color::render(const morda::Matr4r& matrix)const{
	applySimpleAlphaBlending();
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	auto& r = morda::inst().renderer();
	r.shader->colorPos->render(matr, *r.posQuad01VAO, this->color());
}
