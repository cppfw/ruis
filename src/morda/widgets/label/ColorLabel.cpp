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
	applySimpleAlphaBlending();
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	if(this->gradient){
//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		auto& s = App::inst().shaders.clrPosShader;
		s.setMatrix(matr);
		this->gradient->render(s);
	}else{
		ColorPosShader& s = App::inst().shaders.colorPosShader;

		s.setColor(this->color());
		s.setMatrix(matr);
		s.render(utki::wrapBuf(s.quad01Fan));
	}
}
