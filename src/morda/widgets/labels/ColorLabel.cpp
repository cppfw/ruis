#include "ColorLabel.hpp"

#include "../../App.hpp"


using namespace morda;


ColorLabel::ColorLabel(const stob::Node* chain) :
		Widget(chain),
		ColorWidget(chain)
{}



void ColorLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	ColorPosShader& s = App::Inst().Shaders().colorPosShader;

	s.SetColor(this->color());
	s.SetMatrix(matr);
	s.render(s.quad01Fan);
}
