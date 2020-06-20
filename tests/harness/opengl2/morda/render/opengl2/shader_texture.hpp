#pragma once

#include <morda/render/texturing_shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class OpenGL2ShaderTexture :
		public morda::texturing_shader,
		public OpenGL2ShaderBase
{
	GLint textureUniform;
public:
	OpenGL2ShaderTexture();
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, const morda::texture_2d& tex)const override;
};

}}
