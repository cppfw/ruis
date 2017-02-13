#pragma once

#include <morda/render/ShaderPosTex.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{

class OpenGLES2ShaderPosTex : public morda::ShaderPosTex, public OpenGLES2Shader{
	GLint textureUniform;
public:
	OpenGLES2ShaderPosTex();
	
	void render(const kolme::Matr4f& m, const morda::Texture2D& tex, const morda::VertexArray& va) override;
};

}
