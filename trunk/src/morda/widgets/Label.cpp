#include "Label.hpp"

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
	
	//TODO:
//	if(const stob::Node* p = description.GetProperty("gravity")){
//		
//	}
}



void Label::SetText(const std::string& text){
	this->text = text;
	
	this->bb = this->font->Fnt().StringBoundingBox(this->text);
	
	this->RelayoutNeeded();
}



//override
morda::Vec2f Label::ComputeMinDim()const throw(){
	return this->bb.d;
}



void Label::SetGravity(E_Gravity gravity){
	this->gravity = gravity;
	
	//TODO:
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
	matr.Translate(-this->bb.p);
	this->font->Fnt().RenderString(s, matr, this->text);
}
