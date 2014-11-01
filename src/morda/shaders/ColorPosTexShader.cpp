#include "ColorPosTexShader.hpp"


using namespace morda;


ColorPosTexShader::ColorPosTexShader() :
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
		
						uniform highp vec4 uniformColor;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(textureNumber, tc0) * uniformColor;
						}
					)qwertyuiop"
			)
{}
