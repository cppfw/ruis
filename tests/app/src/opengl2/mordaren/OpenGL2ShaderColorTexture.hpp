#pragma once

#include <morda/render/ShaderColorTexture.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{	

class OpenGL2ShaderColorTexture :
		public morda::ShaderColorTexture,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorTexture();
	
	OpenGL2ShaderColorTexture(const OpenGL2ShaderColorTexture&) = delete;
	OpenGL2ShaderColorTexture& operator=(const OpenGL2ShaderColorTexture&) = delete;
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color, const morda::Texture2D& tex) override;
};

}