#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label(const stob::Node* desc) :
		Widget(desc),
		GravitatingWidget(desc),
		PaddedWidget(desc),
		TextWidget(desc)
{
	if(!desc){
		return;
	}
	
	if(const stob::Node* p = desc->GetProperty("text")){
		this->SetText(p->Value());
	}
}



void Label::SetText(const std::string& text){
	this->text = text;
	
	this->SetLines({ting::utf8::ToUTF32(text)});
}




morda::Vec2r Label::ComputeMinDim()const NOEXCEPT{
	LeftBottomRightTop padding = this->Padding();
//	TRACE(<< "Label::ComputeMinDim(): padding = (" << padding.left << ", " << padding.top << ", " << padding.right << ", " << padding.bottom << ")" << std::endl)
	return this->TextWidget::ComputeMinDim() + padding.lb + padding.rt;
}



//override
void Label::OnResize(){
	Vec2r p = this->gravity().PosForRect(*this, this->TextBoundingBox().d);
	
	this->pivot = p - this->TextBoundingBox().p;
	
	//TODO: change to round vec
	for(unsigned i = 0; i != 2; ++i){
		this->pivot[i] = ting::math::Round(this->pivot[i]);
	}
}



//override
void Label::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Translate(this->pivot);
	this->TextWidget::Render(matr);
}
