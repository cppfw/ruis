#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label(const stob::Node* desc) :
		Widget(desc),
		SingleLineTextWidget(desc)
{
//	if(!desc){
//		return;
//	}
}




//override
void Label::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Translate(-this->TextBoundingBox().p);
	this->Font().RenderString(morda::App::Inst().Shaders().posTexShader, matr, this->Text());
}
