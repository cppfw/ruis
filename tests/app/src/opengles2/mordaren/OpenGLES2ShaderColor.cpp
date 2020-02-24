#include "OpenGLES2ShaderColor.hpp"

using namespace mordaren;

OpenGLES2ShaderColor::OpenGLES2ShaderColor() :
		OpenGLES2ShaderBase(
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

void OpenGLES2ShaderColor::render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const{
	this->bind();
	
	this->setUniform4f(this->colorUniform, color.x, color.y, color.z, color.w);
	
	this->OpenGLES2ShaderBase::render(m, va);
}
