#pragma once

#include <morda/render/coloring_shader.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace morda{ namespace render_opengles2{

class OpenGLES2ShaderColor :
		public morda::coloring_shader,
		public OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColor();
	
	OpenGLES2ShaderColor(const OpenGLES2ShaderColor&) = delete;
	OpenGLES2ShaderColor& operator=(const OpenGLES2ShaderColor&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va, r4::vec4f color)const override;
};

}}
