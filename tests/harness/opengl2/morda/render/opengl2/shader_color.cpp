#include "shader_color.hpp"

using namespace morda::render_opengl2;

shader_color::shader_color() :
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

void shader_color::render(const r4::matrix4<float>& m, const morda::vertex_array& va, r4::vector4<float> color)const{
	this->bind();
	
	this->setUniform4f(this->colorUniform, color.x(), color.y(), color.z(), color.w());
	
	this->OpenGL2ShaderBase::render(m, va);
}
