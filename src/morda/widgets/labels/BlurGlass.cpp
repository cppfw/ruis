#include "BlurGlass.hpp"

#include "../../render/Texture2D.hpp"

#include "../../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::render(const morda::Matr4r& matrix)const{
	kolme::Recti viewPortRect = this->computeViewportRect(matrix);
	
	if(!viewPortRect.d.isPositive()){
		return;
	}
	
	Texture2D texture(this->rect().d.to<unsigned>(), 3, Render::ETexFilter::NEAREST, Render::ETexFilter::NEAREST);
	ASSERT(texture.isBound())
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	morda::SimpleBlurPosTexShader &s = App::inst().shaders().simpleBlurPosTexShader;

	s.SetMatrix(matr);
	
	s.setTextureStep(kolme::Vec2f(1).compDiv(viewPortRect.d.to<float>()));
	
	for(unsigned i = 0; i != 3; ++i){
		Render::copyColorBufferToTexture(kolme::Vec2i(0), viewPortRect);
		
		s.render(utki::wrapBuf(morda::PosShader::quad01Fan), utki::wrapBuf(s.quadFanTexCoordsUpsideDown));
	}
}
