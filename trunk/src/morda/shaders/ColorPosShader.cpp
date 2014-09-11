#include "ColorPosShader.hpp"



using namespace morda;



ColorPosShader::ColorPosShader() :
		Shader(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 vertex;
						uniform lowp vec4 uniformColor;
						uniform highp mat4 matrix;
						varying lowp vec4 clr;
						void main(void){
							gl_Position = matrix * vertex;
							clr = uniformColor;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						varying lowp vec4 clr;
						void main(void){
							gl_FragColor = clr;
						}
				)qwertyuiop"
			)
{}
