#pragma once

#include <morda/render/ShaderColorPosTex.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{	

class OpenGLES2ShaderColorPosTex : public morda::ShaderColorPosTex, public OpenGLES2Shader{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPosTex();
	
	OpenGLES2ShaderColorPosTex(const OpenGLES2ShaderColorPosTex&) = delete;
	OpenGLES2ShaderColorPosTex& operator=(const OpenGLES2ShaderColorPosTex&) = delete;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D& tex, kolme::Vec4f color, const morda::VertexArray& va) override;
};

}