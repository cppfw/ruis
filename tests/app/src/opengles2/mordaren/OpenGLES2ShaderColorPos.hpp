#pragma once

#include <morda/render/ShaderColor.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{	

class OpenGLES2ShaderColorPos :
		public morda::ShaderColor,
		public OpenGLES2Shader
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPos();
	
	OpenGLES2ShaderColorPos(const OpenGLES2ShaderColorPos&) = delete;
	OpenGLES2ShaderColorPos& operator=(const OpenGLES2ShaderColorPos&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color) override;
};

}
