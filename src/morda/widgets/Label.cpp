#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label(const stob::Node* desc) :
		Widget(desc),
		GravitatingWidget(desc),
		SingleLineTextWidget(desc)
{
//	if(!desc){
//		return;
//	}
}



//override
void Label::OnResize(){
	auto bb = this->Font().StringBoundingBox(this->Text());
	
	Vec2r p = this->gravity().PosForRect(this->Rect().d, bb.d);
	
	this->pivot = (p - bb.p).Rounded();
}



//override
void Label::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Translate(this->pivot);
	this->Font().RenderString(matrix, this->Text());
}
