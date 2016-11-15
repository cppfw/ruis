#include "OpenGL2ShaderPosClr.hpp"



OpenGL2ShaderPosClr::OpenGL2ShaderPosClr() :
		OpenGL2Shader(
				R"qwertyuiop(
						uniform mat4 matrix;

						attribute vec4 a0;
						attribute vec4 a1;

						varying vec4 color_varying;

						void main(void){
							gl_Position = matrix * a0;
							color_varying = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(		
						varying vec4 color_varying;
						
						void main(void){
							gl_FragColor = color_varying;
						}
					)qwertyuiop"
			)
{
}

void OpenGL2ShaderPosClr::render(const kolme::Matr4f& m, const morda::VertexArray& va) const {
	this->bind();
	
	this->OpenGL2Shader::render(m, va);
}

