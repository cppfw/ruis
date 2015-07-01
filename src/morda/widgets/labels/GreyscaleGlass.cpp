#include "GreyscaleGlass.hpp"

#include "../../util/GLTexture.hpp"

#include "../../App.hpp"


using namespace morda;



GreyscaleGlass::GreyscaleGlass(const stob::Node* chain) :
		Widget(chain)
{}



void GreyscaleGlass::Render(const morda::Matr4r& matrix) const{
	GLTexture texture(this->Rect().d.ConvertTo<unsigned>(), 3, GL_NEAREST, GL_NEAREST);
	//texture is bound right after creation
	
	Rect2i viewPortRect = this->ComputeViewportRect(matrix);
	
	if(!viewPortRect.d.IsPositive()){
		return;
	}
	
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
//	Shader::AssertOpenGLNoError();
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	morda::PosTexShader &s = App::Inst().Shaders().simpleGrayscalePosTexShader;
	s.Bind();
	s.SetMatrix(matr);
	
	s.render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
}
