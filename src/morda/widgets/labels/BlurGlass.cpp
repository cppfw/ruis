#include "BlurGlass.hpp"

#include "../../render/Texture2D.hpp"

#include "../../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::render(const morda::Matr4r& matrix)const{
	Rect2i viewPortRect = this->ComputeViewportRect(matrix);
	
	if(!viewPortRect.d.IsPositive()){
		return;
	}
	
	Texture2D texture(this->rect().d.to<unsigned>(), 3, Render::ETexFilter::NEAREST, Render::ETexFilter::NEAREST);
	//texture is bound right after creation
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->rect().d);

	morda::SimpleBlurPosTexShader &s = App::Inst().Shaders().simpleBlurPosTexShader;

	s.SetMatrix(matr);
	
	s.SetTextureStep(Vec2f(1).CompDiv(viewPortRect.d.to<float>()));
	
	for(unsigned i = 0; i != 3; ++i){
		Render::copyColorBufferToTexture(Vec2i(0), viewPortRect);
		
		s.render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
	}
}
