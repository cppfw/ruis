#include "TextLabel.hpp"


#include "../../Morda.hpp"
#include "../../util/util.hpp"



using namespace morda;



TextLabel::TextLabel(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{}



void TextLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.translate(-this->textBoundingBox().p.x, -this->font().boundingBox().p.y);
	
	this->font().renderString(matr, morda::colorToVec4f(this->color()), this->text());
}
