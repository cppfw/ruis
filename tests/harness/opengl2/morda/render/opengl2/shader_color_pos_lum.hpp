#pragma once

#include <morda/render/coloring_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class OpenGL2ShaderColorPosLum :
		public morda::coloring_shader,
		private morda::render_opengl2::OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosLum();
	
	OpenGL2ShaderColorPosLum(const OpenGL2ShaderColorPosLum&) = delete;
	OpenGL2ShaderColorPosLum& operator=(const OpenGL2ShaderColorPosLum&) = delete;
	
	using morda::coloring_shader::render;
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

}}
