#pragma once

#include <morda/render/ShaderTexture.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{

class OpenGL2ShaderTexture :
		public morda::ShaderTexture,
		public OpenGL2ShaderBase
{
	GLint textureUniform;
public:
	OpenGL2ShaderTexture();
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, const morda::Texture2D& tex)const override;
};

}
