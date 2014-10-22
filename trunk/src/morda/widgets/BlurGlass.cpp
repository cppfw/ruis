#include "BlurGlass.hpp"

#include "../util/GLTexture.hpp"

#include "../App.hpp"


using namespace morda;



BlurGlass::BlurGlass(const stob::Node* chain) :
		Widget(chain)
{}


void BlurGlass::Render(const morda::Matr4r& matrix)const{
	GLTexture texture(this->Rect().d.ConvertTo<unsigned>(), 4, GL_NEAREST, GL_NEAREST);
	//texture is bound right after creation
	
	Rect2i viewPortRect = this->ComputeViewportRect(matrix);
	
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
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	morda::PosTexShader &s = App::Inst().Shaders().simpleBlurPosTexShader;
	s.Bind();
	s.SetMatrix(matr);
	
	s.Render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
}
