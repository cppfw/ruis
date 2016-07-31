#include "TextLabel.hpp"


#include "../../Morda.hpp"



using namespace morda;



TextLabel::TextLabel(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{}



void TextLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.translate(-this->textBoundingBox().p.x, -this->font().boundingBox().p.y);
	
	PosTexShader& s = [this]() -> PosTexShader&{
		if(this->color() == 0xffffffff){//if white
			return morda::Morda::inst().shaders.posTexShader;
		}else{
			ColorPosTexShader& s = morda::Morda::inst().shaders.colorPosTexShader;

			s.setColor(this->color());
			return s;
		}
	}();
	
	this->font().renderString(s, matr, this->text());
}
