#include "PosTexShader.hpp"



using namespace morda;



decltype(PosTexShader::quadFanTexCoords) PosTexShader::quadFanTexCoords = {{
	Vec2f(0, 0), Vec2f(1, 0), Vec2f(1, 1), Vec2f(0, 1)
}};



PosTexShader::PosTexShader() :
		Shader(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 vertex;
						attribute highp vec2 textureCoord;
						uniform highp mat4 matrix;
						varying highp vec2 texCoord;
						void main(void){
							gl_Position = matrix * vertex;
							texCoord = textureCoord;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
						uniform sampler2D textureNumber;
						varying highp vec2 texCoord;
						void main(void){
							gl_FragColor = texture2D(textureNumber, texCoord);
						}
					)qwertyuiop"
			)
{
	this->texCoordAttr = this->GetAttribute("textureCoord");
	this->texNumberUniform = this->GetUniform("textureNumber");
}
