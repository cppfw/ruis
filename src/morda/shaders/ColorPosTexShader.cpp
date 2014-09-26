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

						attribute highp vec4 vertex;
						attribute highp vec2 textureCoord;

						uniform highp mat4 matrix;
						uniform highp vec4 uniformColor;

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
						uniform highp vec4 uniformColor;
		
						varying highp vec2 texCoord;
		
						void main(void){
							gl_FragColor = texture2D(textureNumber, texCoord) * uniformColor;
						}
					)qwertyuiop"
			)
{}
