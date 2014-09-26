#include "ColorLabel.hpp"

#include "../App.hpp"


using namespace morda;


ColorLabel::ColorLabel(const stob::Node* desc) :
		Widget(desc),
		ColorWidget(desc)
{}



void ColorLabel::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	ColorPosShader& s = App::Inst().Shaders().colorPosShader;
	s.Bind();
	s.SetColor(this->Color());
	s.SetMatrix(matr);
	s.Render(s.quad01Fan, Shader::EMode::TRIANGLE_FAN);
}
