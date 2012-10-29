#include "Label.hpp"

#include <ting/math.hpp>

#include "../App.hpp"



using namespace morda;



Label::Label(){
	this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
}


void Label::ApplyDescription(const stob::Node& description){
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
	
	this->bb = this->font->Fnt().StringBoundingBox(this->text);
	
	this->RelayoutNeeded();
}



//override
morda::Vec2f Label::ComputeMinDim()const throw(){
	LeftTopRightBottom padding = this->GetPadding();
	TRACE(<< "Label::ComputeMinDim(): padding = (" << padding.left << ", " << padding.top << ", " << padding.right << ", " << padding.bottom << ")" << std::endl)
	return this->bb.d + morda::Vec2f(padding.left + padding.right, padding.top + padding.bottom);
}



//override
void Label::OnResize(){
	morda::Vec2f paddingLow(this->GetPadding().left, this->GetPadding().bottom);
	morda::Vec2f paddingHigh(this->GetPadding().right, this->GetPadding().top);
	//update pivot
	for(unsigned i = 0; i != 2; ++i){
		switch(this->GetGravity()[i]){
			case Gravity::LEFT:
//			case Gravity::BOTTOM:
				this->pivot[i] = -this->bb.p[i] + paddingLow[i];
				break;
			case Gravity::RIGHT:
//			case Gravity::TOP:
				this->pivot[i] = this->Rect().d[i] - paddingHigh[i] - (this->bb.p[i] + this->bb.d[i]);
				break;
			default:
			case Gravity::CENTER:
				this->pivot[i] = paddingLow[i] + (this->Rect().d[i] - paddingLow[i] - paddingHigh[i] - this->bb.d[i]) / 2 - this->bb.p[i];
				break;
		}
		this->pivot[i] = ting::math::Round(this->pivot[i]);
	}
}



//override
void Label::Render(const morda::Matr4f& matrix)const{
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	this->Widget::Render(matrix);
#endif
	
	//render text
	morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
	s.Bind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	morda::Matr4f matr(matrix);
	matr.Translate(this->pivot);
	this->font->Fnt().RenderString(s, matr, this->text);
}
