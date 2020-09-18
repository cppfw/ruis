#pragma once

#include <morda/render/coloring_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class shader_color_pos_lum :
		public morda::coloring_shader,
		private morda::render_opengl2::OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	shader_color_pos_lum();
	
	shader_color_pos_lum(const shader_color_pos_lum&) = delete;
	shader_color_pos_lum& operator=(const shader_color_pos_lum&) = delete;
	
	using morda::coloring_shader::render;
	
	void render(const r4::matrix4<float>& m, const morda::vertex_array& va, r4::vector4<float> color)const override;
};

}}
