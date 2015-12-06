#include "ColorLabel.hpp"

#include "../../App.hpp"


using namespace morda;


ColorLabel::ColorLabel(const stob::Node* chain) :
		Widget(chain),
		ColorWidget(chain)
{}



void ColorLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	ColorPosShader& s = App::inst().shaders().colorPosShader;

	s.setColor(this->color());
	s.SetMatrix(matr);
	s.render(utki::wrapBuf(s.quad01Fan));
}
