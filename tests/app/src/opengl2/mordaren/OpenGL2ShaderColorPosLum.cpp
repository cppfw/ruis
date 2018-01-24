#include "OpenGL2ShaderColorPosLum.hpp"


OpenGL2ShaderColorPosLum::OpenGL2ShaderColorPosLum() :
		OpenGL2ShaderBase(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 a0;
						attribute highp float a1;

						uniform highp mat4 matrix;

						varying highp float lum;

						void main(void){
							gl_Position = matrix * a0;
							lum = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						uniform lowp vec4 uniformColor;
		
						varying highp float lum;
		
						void main(void){
							gl_FragColor = vec4(uniformColor.x, uniformColor.y, uniformColor.z, uniformColor.w * lum);
						}
					)qwertyuiop"
			)
{
	this->colorUniform = this->getUniform("uniformColor");
}

void OpenGL2ShaderColorPosLum::render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color)const {
	this->bind();
	
	this->setUniform4f(this->colorUniform, color.x, color.y, color.z, color.w);
	
	this->OpenGL2ShaderBase::render(m, va);
}
