#pragma once

#include <morda/render/ShaderTexture.hpp>

#include "OpenGLES2Shader.hpp"

namespace mordaren{

class OpenGLES2ShaderPosTex :
		public morda::ShaderTexture,
		public OpenGLES2Shader
{
	GLint textureUniform;
public:
	OpenGLES2ShaderPosTex();
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, const morda::Texture2D& tex) override;
};

}
