#include "GreyscaleGlass.hpp"


#include "../App.hpp"


using namespace morda;



GreyscaleGlass::GreyscaleGlass(const stob::Node* chain) :
		Widget(chain)
{
	this->tex = morda::App::Inst().resMan.Load<ResTexture>("tex_sample");
}



void GreyscaleGlass::Render(const morda::Matr4r& matrix) const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	morda::PosTexShader &s = App::Inst().Shaders().simpleGrayscalePosTexShader;
	s.Bind();
	s.SetMatrix(matr);
	this->tex->Tex().Bind();
	
	s.Render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
}
