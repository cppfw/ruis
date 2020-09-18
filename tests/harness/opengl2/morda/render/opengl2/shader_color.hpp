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
	

	void render(const r4::matrix4<float>& m, const morda::vertex_array& va, r4::vector4<float> color)const override;
};

}}
