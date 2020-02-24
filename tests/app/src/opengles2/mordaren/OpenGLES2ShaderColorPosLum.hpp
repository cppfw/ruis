#pragma once

#include <morda/render/coloring_shader.hpp>
#include "OpenGLES2ShaderBase.hpp"

class OpenGLES2ShaderColorPosLum :
		public morda::coloring_shader,
		private mordaren::OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPosLum();
	
	OpenGLES2ShaderColorPosLum(const OpenGLES2ShaderColorPosLum&) = delete;
	OpenGLES2ShaderColorPosLum& operator=(const OpenGLES2ShaderColorPosLum&) = delete;
	
	using morda::coloring_shader::render;
	
	void render(const r4::mat4f& m, const morda::VertexArray& va, r4::vec4f color)const override;
};

