#pragma once

#include <morda/render/Shader.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace mordaren{

class OpenGLES2ShaderPosClr :
		public morda::Shader,
		public OpenGLES2ShaderBase
{
public:
	OpenGLES2ShaderPosClr();
	
	OpenGLES2ShaderPosClr(const OpenGLES2ShaderPosClr&) = delete;
	OpenGLES2ShaderPosClr& operator=(const OpenGLES2ShaderPosClr&) = delete;
	

	void render(const r4::mat4f& m, const morda::VertexArray& va) const override;
};

}
