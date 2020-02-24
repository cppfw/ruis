#pragma once

#include <morda/render/texturing_shader.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace mordaren{

class OpenGLES2ShaderPosTex :
		public morda::texturing_shader,
		public OpenGLES2ShaderBase
{
	GLint textureUniform;
public:
	OpenGLES2ShaderPosTex();
	
	void render(const r4::mat4f& m, const morda::VertexArray& va, const morda::texture_2d& tex)const override;
};

}
