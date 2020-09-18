#include "shader_texture.hpp"

#include "texture_2d.hpp"
#include "vertex_buffer.hpp"
#include "vertex_array.hpp"
#include "index_buffer.hpp"
#include "util.hpp"

using namespace morda::render_opengl2;

shader_texture::shader_texture() :
		OpenGL2ShaderBase(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 a0; //position

						attribute highp vec2 a1; //texture coordinates

						uniform highp mat4 matrix;

						varying highp vec2 tc0;

						void main(void){
							gl_Position = matrix * a0;
							tc0 = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						uniform sampler2D texture0;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(texture0, tc0);
						}
					)qwertyuiop"
			)
{
	this->textureUniform = this->getUniform("texture0");
}


void shader_texture::render(const r4::matrix4<float>& m, const morda::vertex_array& va, const morda::texture_2d& tex)const{
	static_cast<const texture_2d&>(tex).bind(0);
	this->bind();
	
	this->OpenGL2ShaderBase::render(m, va);
}

