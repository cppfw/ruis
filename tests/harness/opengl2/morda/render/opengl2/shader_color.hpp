#pragma once

#include <morda/render/coloring_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class OpenGL2ShaderColor :
		public morda::coloring_shader,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColor();
	
	OpenGL2ShaderColor(const OpenGL2ShaderColor&) = delete;
	OpenGL2ShaderColor& operator=(const OpenGL2ShaderColor&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

}}
