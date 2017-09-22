#pragma once

#include <morda/render/ShaderTexture.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace mordaren{

class OpenGLES2ShaderPosTex :
		public morda::ShaderTexture,
		public OpenGLES2ShaderBase
{
	GLint textureUniform;
public:
	OpenGLES2ShaderPosTex();
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, const morda::Texture2D& tex) override;
};

}
