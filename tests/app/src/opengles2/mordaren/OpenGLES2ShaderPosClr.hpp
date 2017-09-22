#pragma once

#include <morda/render/Shader.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{

class OpenGLES2ShaderPosClr :
		public morda::Shader,
		public OpenGLES2Shader
{
public:
	OpenGLES2ShaderPosClr();
	
	OpenGLES2ShaderPosClr(const OpenGLES2ShaderPosClr&) = delete;
	OpenGLES2ShaderPosClr& operator=(const OpenGLES2ShaderPosClr&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va) const override;
};

}
