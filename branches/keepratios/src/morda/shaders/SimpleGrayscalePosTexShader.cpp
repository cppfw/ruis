#include "SimpleGrayscalePosTexShader.hpp"



using namespace morda;



SimpleGrayscalePosTexShader::SimpleGrayscalePosTexShader() :
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
							lowp vec4 c = texture2D(textureNumber, texCoord);
							lowp float g = (c.r + c.g + c.b) / 3.0;
							gl_FragColor = vec4(g, g, g, c.a);
						}
					)qwertyuiop"
			)
{}
