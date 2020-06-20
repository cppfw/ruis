#pragma once

#include <morda/render/coloring_texturing_shader.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace morda{	

class OpenGLES2ShaderColorPosTex :
		public morda::coloring_texturing_shader,
		public OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPosTex();
	
	OpenGLES2ShaderColorPosTex(const OpenGLES2ShaderColorPosTex&) = delete;
	OpenGLES2ShaderColorPosTex& operator=(const OpenGLES2ShaderColorPosTex&) = delete;
	
	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color, const morda::texture_2d& tex)const override;
};

}