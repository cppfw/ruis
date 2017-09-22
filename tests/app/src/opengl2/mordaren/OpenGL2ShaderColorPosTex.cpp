#include "OpenGL2ShaderColorPosTex.hpp"

#include "OpenGL2Texture2D.hpp"

using namespace mordaren;

OpenGL2ShaderColorPosTex::OpenGL2ShaderColorPosTex() :
		OpenGL2ShaderBase(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 a0;

						attribute highp vec2 a1;

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
		
						uniform highp vec4 uniformColor;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(texture0, tc0) * uniformColor;
						}
					)qwertyuiop"
			)
{
	this->colorUniform = this->getUniform("uniformColor");
}

void OpenGL2ShaderColorPosTex::render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color, const morda::Texture2D& tex) {
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->setUniform4f(this->colorUniform, color.x, color.y, color.z, color.w);
	
	this->OpenGL2ShaderBase::render(m, va);
}
