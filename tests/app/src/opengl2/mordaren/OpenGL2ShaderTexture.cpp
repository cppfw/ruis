#include "OpenGL2ShaderTexture.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"
#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2_util.hpp"

using namespace mordaren;

OpenGL2ShaderTexture::OpenGL2ShaderTexture() :
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


void OpenGL2ShaderTexture::render(const r4::mat4f& m, const morda::vertex_array& va, const morda::texture_2d& tex)const{
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->OpenGL2ShaderBase::render(m, va);
}

