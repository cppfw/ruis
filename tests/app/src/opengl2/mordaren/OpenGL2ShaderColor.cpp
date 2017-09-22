#include "OpenGL2ShaderColor.hpp"

using namespace mordaren;

OpenGL2ShaderColor::OpenGL2ShaderColor() :
		OpenGL2ShaderBase(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 a0;

						uniform highp mat4 matrix;
						
						void main(void){
							gl_Position = matrix * a0;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						uniform lowp vec4 uniformColor;
		
						void main(void){
							gl_FragColor = uniformColor;
						}
				)qwertyuiop"
			)
{
	this->colorUniform = this->getUniform("uniformColor");
}

void OpenGL2ShaderColor::render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color) {
	this->bind();
	
	this->setUniform4f(this->colorUniform, color.x, color.y, color.z, color.w);
	
	this->OpenGL2ShaderBase::render(m, va);
}
