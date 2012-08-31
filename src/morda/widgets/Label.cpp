#include "Label.hpp"

#include "../App.hpp"


using namespace morda;



void Label::ApplyProperties(const stob::Node* properties){
	if(!properties){
		this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
		return;
	}

	//NOTE: font must be loaded before setting the text because it gets the string bounding box from the font.
	if(const stob::Node* p = properties->GetProperty("font")){
		this->font = App::Inst().ResMan().Load<morda::ResFont>(p->Value());
	}else{
		this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
	}
	
	if(const stob::Node* p = properties->GetProperty("text")){
		this->SetText(p->Value());
	}
}



void Label::SetText(const std::string& text){
	this->text = text;
	
	morda::Rect2f bb = this->font->Fnt().StringBoundingBox(this->text);
	
	if(this->Rect().d.x < bb.d.x || this->Rect().d.y < bb.d.y){
		this->Resize(bb.d);
		this->pivot = -bb.p;
	}else{
		//TODO: handle gravity
		this->pivot = -bb.p;
	}
}



void Label::SetGravity(E_Gravity gravity){
	this->gravity = gravity;
	
	//TODO:
}



//override
void Label::Render(const morda::Matr4f& matrix)const{
//	{
//		morda::Matr4f matr(matrix);
//		matr.Scale(this->Rect().d);
//		SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
//		s.Bind();
//		s.SetColor(morda::Vec3f(0.5, 0.5, 0.5));
//		s.SetMatrix(matr);
//		s.DrawQuad01();
//	}
	
	//render text
	morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
	s.Bind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	morda::Matr4f matr(matrix);
	matr.Translate(this->pivot);
	this->font->Fnt().RenderString(s, matr, this->text);
}
