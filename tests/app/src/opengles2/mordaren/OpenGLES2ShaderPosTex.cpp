#include "OpenGLES2ShaderPosTex.hpp"

#include "OpenGLES2Texture2D.hpp"
#include "OpenGLES2VertexBuffer.hpp"
#include "OpenGLES2VertexArray.hpp"
#include "OpenGLES2IndexBuffer.hpp"

#include "OpenGLES2_util.hpp"

using namespace mordaren;

OpenGLES2ShaderPosTex::OpenGLES2ShaderPosTex() :
		OpenGLES2ShaderBase(
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


void OpenGLES2ShaderPosTex::render(const r4::mat4f& m, const morda::VertexArray& va, const morda::texture_2d& tex)const{
	ASSERT(dynamic_cast<const OpenGLES2Texture2D*>(&tex))
	static_cast<const OpenGLES2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->OpenGLES2ShaderBase::render(m, va);
}

