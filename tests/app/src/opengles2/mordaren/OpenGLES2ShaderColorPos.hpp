#pragma once

#include <morda/render/ShaderColorPos.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{	

class OpenGLES2ShaderColorPos : public morda::ShaderColorPos, public OpenGLES2Shader{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPos();
	
	OpenGLES2ShaderColorPos(const OpenGLES2ShaderColorPos&) = delete;
	OpenGLES2ShaderColorPos& operator=(const OpenGLES2ShaderColorPos&) = delete;
	

	void render(const kolme::Matr4f& m, kolme::Vec4f color, const morda::VertexArray& va) override;
};

}
