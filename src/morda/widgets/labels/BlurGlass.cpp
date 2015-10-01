#include "BlurGlass.hpp"

#include "../../render/Texture2D.hpp"

#include "../../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::render(const morda::Matr4r& matrix)const{
	Recti viewPortRect = this->ComputeViewportRect(matrix);
	
	if(!viewPortRect.d.isPositive()){
		return;
	}
	
	Texture2D texture(this->rect().d.to<unsigned>(), 3, Render::ETexFilter::NEAREST, Render::ETexFilter::NEAREST);
	//texture is bound right after creation
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	morda::SimpleBlurPosTexShader &s = App::inst().Shaders().simpleBlurPosTexShader;

	s.SetMatrix(matr);
	
	s.SetTextureStep(Vec2f(1).compDiv(viewPortRect.d.to<float>()));
	
	for(unsigned i = 0; i != 3; ++i){
		Render::copyColorBufferToTexture(Vec2i(0), viewPortRect);
		
		s.render(utki::wrapBuf(morda::PosShader::quad01Fan), utki::wrapBuf(s.quadFanTexCoords));
	}
}
