#include "SimpleBlurPosTexShader.hpp"


using namespace morda;



SimpleBlurPosTexShader::SimpleBlurPosTexShader() :
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
						
						uniform highp vec2 texStep;
						
						varying highp vec2 texCoord;
		
						void main(void){
							vec4 c = texture2D(textureNumber, vec2(texCoord.x - texStep.x, texCoord.y - texStep.y)) * 1.0 / 16.0;
							c += texture2D(textureNumber, vec2(texCoord.x, texCoord.y - texStep.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, vec2(texCoord.x + texStep.x, texCoord.y - texStep.y)) * 1.0 / 16.0;
		
							c += texture2D(textureNumber, vec2(texCoord.x - texStep.x, texCoord.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, texCoord) * 4.0 / 16.0;
							c += texture2D(textureNumber, vec2(texCoord.x + texStep.x, texCoord.y)) * 2.0 / 16.0;
		
							c += texture2D(textureNumber, vec2(texCoord.x - texStep.x, texCoord.y + texStep.y)) * 1.0 / 16.0;
							c += texture2D(textureNumber, vec2(texCoord.x, texCoord.y + texStep.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, vec2(texCoord.x + texStep.x, texCoord.y + texStep.y)) * 1.0 / 16.0;
		
							gl_FragColor = c;
						}
					)qwertyuiop"
			)
{
	this->texStepUniform =  this->GetUniform("texStep");
}