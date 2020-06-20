#pragma once

#include <morda/render/coloring_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class coloring_shader :
		public morda::coloring_shader,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	coloring_shader();
	
	coloring_shader(const coloring_shader&) = delete;
	coloring_shader& operator=(const coloring_shader&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

}}
