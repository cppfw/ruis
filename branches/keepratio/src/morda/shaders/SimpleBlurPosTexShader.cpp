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
						
						uniform highp vec2 texStep;
						
						varying highp vec2 tc0;
		
						void main(void){
							lowp vec4 c = texture2D(textureNumber, vec2(tc0.x - texStep.x, tc0.y - texStep.y)) * 1.0 / 16.0;
							c += texture2D(textureNumber, vec2(tc0.x, tc0.y - texStep.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, vec2(tc0.x + texStep.x, tc0.y - texStep.y)) * 1.0 / 16.0;
		
							c += texture2D(textureNumber, vec2(tc0.x - texStep.x, tc0.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, tc0) * 4.0 / 16.0;
							c += texture2D(textureNumber, vec2(tc0.x + texStep.x, tc0.y)) * 2.0 / 16.0;
		
							c += texture2D(textureNumber, vec2(tc0.x - texStep.x, tc0.y + texStep.y)) * 1.0 / 16.0;
							c += texture2D(textureNumber, vec2(tc0.x, tc0.y + texStep.y)) * 2.0 / 16.0;
							c += texture2D(textureNumber, vec2(tc0.x + texStep.x, tc0.y + texStep.y)) * 1.0 / 16.0;
		
							gl_FragColor = c;
						}
					)qwertyuiop"
			)
{
	this->texStepUniform =  this->GetUniform("texStep");
}