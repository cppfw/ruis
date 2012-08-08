#include "Label.hpp"

#include "../App.hpp"


using namespace morda;



TextLabel::TextLabel(){
	this->font = App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
}



void TextLabel::SetText(const std::string& text){
	this->text = text;
	
	tride::Rect2f bb = this->font->Fnt().StringBoundingBox(this->text);
	
	this->pivot = -bb.p;
	this->Resize(bb.d);
}



//override
void TextLabel::Render(const tride::Matr4f& matrix)const{
//	{
//		tride::Matr4f matr(matrix);
//		matr.Scale(this->Rect().d);
//		SimpleSingleColoringShader& s = App::Inst().shaders.simpleSingleColoring;
//		s.Bind();
//		s.SetColor(tride::Vec3f(0.5, 0.5, 0.5));
//		s.SetMatrix(matr);
//		s.DrawQuad01();
//	}
	
	//render text
	morda::SimpleTexturingShader &s = morda::App::Inst().shaders.simpleTexturing;
	s.Bind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	tride::Matr4f matr(matrix);
	matr.Translate(this->pivot);
	this->font->Fnt().RenderString(s, matr, this->text);
}
