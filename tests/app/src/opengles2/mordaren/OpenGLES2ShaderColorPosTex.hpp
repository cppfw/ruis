#pragma once

#include <morda/render/ShaderColorTexture.hpp>

#include "OpenGLES2ShaderBase.hpp"

namespace mordaren{	

class OpenGLES2ShaderColorPosTex :
		public morda::ShaderColorTexture,
		public OpenGLES2ShaderBase
{
	GLint colorUniform;
public:
	OpenGLES2ShaderColorPosTex();
	
	OpenGLES2ShaderColorPosTex(const OpenGLES2ShaderColorPosTex&) = delete;
	OpenGLES2ShaderColorPosTex& operator=(const OpenGLES2ShaderColorPosTex&) = delete;
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color, const morda::Texture2D& tex) override;
};

}