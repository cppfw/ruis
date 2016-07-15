#include "GreyscaleGlass.hpp"

#include "../../render/Texture2D.hpp"

#include "../../App.hpp"


using namespace morda;



GreyscaleGlass::GreyscaleGlass(const stob::Node* chain) :
		Widget(chain)
{}



void GreyscaleGlass::render(const morda::Matr4r& matrix) const{
	Texture2D texture(this->rect().d.to<unsigned>(), 3, Render::TexFilter_e::NEAREST, Render::TexFilter_e::NEAREST);
	ASSERT(texture.isBound())
	
	kolme::Recti viewPortRect = this->computeViewportRect(matrix);
	
	if(!viewPortRect.d.isPositive()){
		return;
	}
	
	Render::copyColorBufferToTexture(kolme::Vec2i(0), viewPortRect);
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	morda::PosTexShader &s = App::inst().shaders.simpleGrayscalePosTexShader;

	s.setMatrix(matr);
	
	s.render(utki::wrapBuf(morda::PosShader::quad01Fan), utki::wrapBuf(s.quadFanTexCoordsUpsideDown));
}
