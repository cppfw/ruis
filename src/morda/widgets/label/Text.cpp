#include "Text.hpp"


#include "../../context.hpp"
#include "../../util/util.hpp"



using namespace morda;



Text::Text(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		SingleLineTextWidget(c, desc)
{}



void Text::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	
	using std::round;
	
	matr.translate(-this->textBoundingBox().p.x, round((this->font().height() + this->font().ascender() - this->font().descender()) / 2));
	
	this->font().renderString(matr, morda::colorToVec4f(this->color()), this->getText());
}
