#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label() : Widget(0){
	this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
}


Label::Label(const stob::Node& description) :
		Widget(description),
		GravitatingWidget(description),
		PaddedWidget(description)
{
	//NOTE: font must be loaded before setting the text because it gets the string bounding box from the font.
	if(const stob::Node* p = description.GetProperty("font")){
		this->font = App::Inst().ResMan().Load<morda::ResFont>(p->Value());
	}else{
		this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
	}
	
	if(const stob::Node* p = description.GetProperty("text")){
		this->SetText(p->Value());
	}
}



void Label::SetText(const std::string& text){
	this->text = text;
	
	this->bb = this->font->font().StringBoundingBox(this->text);
	
	this->SetRelayoutNeeded();
}



//override
morda::Vec2f Label::ComputeMinDim()const throw(){
	LeftBottomRightTop padding = this->Padding();
//	TRACE(<< "Label::ComputeMinDim(): padding = (" << padding.left << ", " << padding.top << ", " << padding.right << ", " << padding.bottom << ")" << std::endl)
	return this->bb.d + padding.lb + padding.rt;
}



//override
void Label::OnResize(){
	Vec2f p = this->gravity().PosForRect(*this, this->bb.d);
	
	this->pivot = p - this->bb.p;
	
	for(unsigned i = 0; i != 2; ++i){
		this->pivot[i] = ting::math::Round(this->pivot[i]);
	}
}



//override
void Label::Render(const morda::Matr4f& matrix)const{
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	this->Widget::Render(matrix);
#endif
	
	morda::Matr4f matr(matrix);
	matr.Translate(this->pivot);
	this->font->font().RenderString(matr, this->text);
}
