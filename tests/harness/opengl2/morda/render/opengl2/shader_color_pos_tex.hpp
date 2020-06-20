#pragma once

#include <morda/render/coloring_texturing_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class shader_color_pos_tex :
		public morda::coloring_texturing_shader,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	shader_color_pos_tex();
	
	shader_color_pos_tex(const shader_color_pos_tex&) = delete;
	shader_color_pos_tex& operator=(const shader_color_pos_tex&) = delete;
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color, const morda::texture_2d& tex)const override;
};

}}
