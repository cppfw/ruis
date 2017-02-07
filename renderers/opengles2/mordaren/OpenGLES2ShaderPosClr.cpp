#include "OpenGLES2ShaderPosClr.hpp"

using namespace mordaren;


OpenGLES2ShaderPosClr::OpenGLES2ShaderPosClr() :
		OpenGLES2Shader(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						uniform highp mat4 matrix;

						attribute highp vec4 a0;
						attribute highp vec4 a1;

						varying highp vec4 color_varying;

						void main(void){
							gl_Position = matrix * a0;
							color_varying = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						varying highp vec4 color_varying;
						
						void main(void){
							gl_FragColor = color_varying;
						}
					)qwertyuiop"
			)
{
}

void OpenGLES2ShaderPosClr::render(const kolme::Matr4f& m, const morda::VertexArray& va) const {
	this->bind();
	
	this->OpenGLES2Shader::render(m, va);
}

