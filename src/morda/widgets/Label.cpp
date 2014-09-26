#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label(const stob::Node* desc) :
		Widget(desc),
		SingleLineTextWidget(desc)
{}




//override
void Label::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Translate(-this->TextBoundingBox().p);
	
	PosTexShader& s = [this]() -> PosTexShader&{
		if(this->Color() == 0xffffffff){//if white
			return morda::App::Inst().Shaders().posTexShader;
		}else{
			ColorPosTexShader& s = morda::App::Inst().Shaders().colorPosTexShader;
			s.Bind();
			s.SetColor(this->Color());
			return s;
		}
	}();
	
	this->Font().RenderString(s, matr, this->Text());
}
