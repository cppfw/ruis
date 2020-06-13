#pragma once

#include <morda/render/coloring_shader.hpp>
#include "OpenGL2ShaderBase.hpp"

class OpenGL2ShaderColorPosLum :
		public morda::coloring_shader,
		private mordaren::OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosLum();
	
	OpenGL2ShaderColorPosLum(const OpenGL2ShaderColorPosLum&) = delete;
	OpenGL2ShaderColorPosLum& operator=(const OpenGL2ShaderColorPosLum&) = delete;
	
	using morda::coloring_shader::render;
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

