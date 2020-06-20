#pragma once

#include <morda/render/coloring_texturing_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class OpenGL2ShaderColorPosTex :
		public morda::coloring_texturing_shader,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosTex();
	
	OpenGL2ShaderColorPosTex(const OpenGL2ShaderColorPosTex&) = delete;
	OpenGL2ShaderColorPosTex& operator=(const OpenGL2ShaderColorPosTex&) = delete;
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color, const morda::texture_2d& tex)const override;
};

}}
