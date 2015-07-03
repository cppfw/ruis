#include "BlurGlass.hpp"

#include "../../util/Texture2D.hpp"

#include "../../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::Render(const morda::Matr4r& matrix)const{
	Rect2i viewPortRect = this->ComputeViewportRect(matrix);
	
	Texture2D texture(this->Rect().d.ConvertTo<unsigned>(), 3, Render::ETexFilter::NEAREST, Render::ETexFilter::NEAREST);
	//texture is bound right after creation
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	morda::SimpleBlurPosTexShader &s = App::Inst().Shaders().simpleBlurPosTexShader;

	s.SetMatrix(matr);
	
	s.SetTextureStep(Vec2f(1).CompDiv(viewPortRect.d.ConvertTo<float>()));
	
	if(!viewPortRect.d.IsPositive()){
		return;
	}
	
	for(unsigned i = 0; i != 3; ++i){
		Render::copyColorBufferToTexture(Vec2i(0), viewPortRect);
		
		s.render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
	}
}
