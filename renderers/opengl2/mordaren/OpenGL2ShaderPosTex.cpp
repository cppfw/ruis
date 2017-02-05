#include "OpenGL2ShaderPosTex.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"
#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2_util.hpp"

using namespace mordaren;

OpenGL2ShaderPosTex::OpenGL2ShaderPosTex() :
		OpenGL2Shader(
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


void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D& tex, const morda::VertexArray& va){
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->OpenGL2Shader::render(m, va);
}

