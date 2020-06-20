#pragma once

#include <morda/render/coloring_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class shader_color :
		public morda::coloring_shader,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	shader_color();
	
	shader_color(const shader_color&) = delete;
	shader_color& operator=(const shader_color&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

}}
