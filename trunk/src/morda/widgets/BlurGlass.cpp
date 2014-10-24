#include "BlurGlass.hpp"

#include "../util/GLTexture.hpp"

#include "../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::Render(const morda::Matr4r& matrix)const{
	Rect2i viewPortRect = this->ComputeViewportRect(matrix);
	
	GLTexture texture(this->Rect().d.ConvertTo<unsigned>(), 4, GL_NEAREST, GL_NEAREST);
	//texture is bound right after creation
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	morda::SimpleBlurPosTexShader &s = App::Inst().Shaders().simpleBlurPosTexShader;
	s.Bind();
	s.SetMatrix(matr);
	
	s.SetTextureStep(Vec2f(1).CompDiv(viewPortRect.d.ConvertTo<float>()));
	
	for(unsigned i = 0; i != 1; ++i){
		glCopyTexSubImage2D(
				GL_TEXTURE_2D,
				0, //level
				0, //xoffset
				0, //yoffset
				viewPortRect.p.x,
				viewPortRect.p.y,
				viewPortRect.d.x,
				viewPortRect.d.y
			);
		
		s.Render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
	}
}
