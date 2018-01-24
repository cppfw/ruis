#pragma once

#include <morda/render/ShaderColor.hpp>
#include "OpenGLES2ShaderBase.hpp"

class OpenGL2ShaderColorPosLum :
		public morda::ShaderColor,
		private mordaren::OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosLum();
	
	OpenGL2ShaderColorPosLum(const OpenGL2ShaderColorPosLum&) = delete;
	OpenGL2ShaderColorPosLum& operator=(const OpenGL2ShaderColorPosLum&) = delete;
	
	using morda::ShaderColor::render;
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color)const override;
};

