#include "OpenGL2ShaderPosClr.hpp"

using namespace mordaren;


OpenGL2ShaderPosClr::OpenGL2ShaderPosClr() :
		OpenGL2ShaderBase(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						uniform highp mat4 matrix;

						attribute highp vec4 a0;
						attribute highp vec4 a1;

						varying highp vec4 color_varying;

						void main(void){
							gl_Position = matrix * a0;
							color_varying = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						varying highp vec4 color_varying;
						
						void main(void){
							gl_FragColor = color_varying;
						}
					)qwertyuiop"
			)
{
}

void OpenGL2ShaderPosClr::render(const r4::mat4f& m, const morda::vertex_array& va) const {
	this->bind();
	
	this->OpenGL2ShaderBase::render(m, va);
}

