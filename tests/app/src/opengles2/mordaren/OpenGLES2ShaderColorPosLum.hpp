#pragma once

#include <morda/render/ShaderColor.hpp>
#include "OpenGLES2ShaderBase.hpp"

class OpenGLES2ShaderColorPosLum :
		public morda::ShaderColor,
		private mordaren::OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPosLum();
	
	OpenGLES2ShaderColorPosLum(const OpenGLES2ShaderColorPosLum&) = delete;
	OpenGLES2ShaderColorPosLum& operator=(const OpenGLES2ShaderColorPosLum&) = delete;
	
	using morda::ShaderColor::render;
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color)const override;
};

