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

						attribute highp vec4 pos;
						attribute highp vec2 texCoord0;

						uniform highp mat4 matrix;

						varying highp vec2 tc0;

						void main(void){
							gl_Position = matrix * pos;
							tc0 = texCoord0;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						uniform sampler2D textureNumber;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(textureNumber, tc0);
						}
					)qwertyuiop"
			)
{
	this->texCoordAttr = this->GetAttribute("texCoord0");
	this->texNumberUniform = this->GetUniform("textureNumber");
}
