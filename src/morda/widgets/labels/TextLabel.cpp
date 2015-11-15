#include "TextLabel.hpp"


#include "../../App.hpp"



using namespace morda;



TextLabel::TextLabel(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{}




//override
void TextLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.translate(-this->textBoundingBox().p.x, -this->Font().BoundingBox().p.y);
	
	PosTexShader& s = [this]() -> PosTexShader&{
		if(this->color() == 0xffffffff){//if white
			return morda::App::inst().Shaders().posTexShader;
		}else{
			ColorPosTexShader& s = morda::App::inst().Shaders().colorPosTexShader;

			s.SetColor(this->color());
			return s;
		}
	}();
	
	this->Font().RenderString(s, matr, utki::wrapBuf(this->text()));
}
