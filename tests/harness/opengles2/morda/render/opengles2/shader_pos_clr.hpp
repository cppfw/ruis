#pragma once

#include <morda/render/shader.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace morda{ namespace render_opengles2{

class OpenGLES2ShaderPosClr :
		public morda::shader,
		public OpenGLES2ShaderBase
{
public:
	OpenGLES2ShaderPosClr();
	
	OpenGLES2ShaderPosClr(const OpenGLES2ShaderPosClr&) = delete;
	OpenGLES2ShaderPosClr& operator=(const OpenGLES2ShaderPosClr&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va) const override;
};

}}
