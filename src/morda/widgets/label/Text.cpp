#include "Text.hpp"


#include "../../Morda.hpp"
#include "../../util/util.hpp"



using namespace morda;



Text::Text(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{}



void Text::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.translate(-this->textBoundingBox().p.x, (this->font().height() + this->font().ascender() - this->font().descender()) / 2);
	
	this->font().renderString(matr, morda::colorToVec4f(this->color()), this->text());
}
