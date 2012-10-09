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
	
	this->SetGravity(Gravity::FromLayout(description));
}



void Label::SetText(const std::string& text){
	this->text = text;
	
	this->bb = this->font->Fnt().StringBoundingBox(this->text);
	
	this->UpdatePivot();
	
	this->RelayoutNeeded();
}



//override
morda::Vec2f Label::ComputeMinDim()const throw(){
	return this->bb.d;
}



//override
void Label::OnResize(){
	this->UpdatePivot();
}



void Label::SetGravity(Gravity gravity){
	this->gravity = gravity;
	
	this->UpdatePivot();
}



void Label::UpdatePivot(){
	switch(this->gravity.hori){
		case Gravity::LEFT:
			this->pivot.x = -this->bb.p.x;
			break;
		case Gravity::RIGHT:
			this->pivot.x = this->Rect().d.x - this->bb.Right();
			break;
		default:
		case Gravity::CENTER:
			this->pivot.x = (this->Rect().d.x - this->bb.d.x) / 2 - this->bb.p.x;
			break;
	}
	
	switch(this->gravity.vert){
		case Gravity::BOTTOM:
			this->pivot.y = -this->bb.p.y;
			break;
		case Gravity::TOP:
			this->pivot.y = this->Rect().d.y - this->bb.Top();
			break;
		default:
		case Gravity::CENTER:
			this->pivot.y = (this->Rect().d.y - this->bb.d.y) / 2 - this->bb.p.y;
			break;
	}
	
	this->pivot.x = ting::math::Round(this->pivot.x);
	this->pivot.y = ting::math::Round(this->pivot.y);
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
