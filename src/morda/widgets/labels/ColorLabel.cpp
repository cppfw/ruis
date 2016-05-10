#include "ColorLabel.hpp"

#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;


ColorLabel::ColorLabel(const stob::Node* chain) :
		Widget(chain),
		ColorWidget(chain)
{
	if(auto p = getProperty(chain, "gradient")){
		this->gradient = App::inst().resMan.load<ResGradient>(p->value());
	}else{
		ASSERT(!this->gradient)
	}
}



void ColorLabel::render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	if(this->gradient){
		this->gradient->render(matr, App::inst().shaders().clrPosShader);
	}else{
		ColorPosShader& s = App::inst().shaders().colorPosShader;

		s.setColor(this->color());
		s.setMatrix(matr);
		s.render(utki::wrapBuf(s.quad01Fan));
	}
}
