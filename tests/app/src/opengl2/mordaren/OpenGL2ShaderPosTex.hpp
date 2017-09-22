#pragma once

#include <morda/render/ShaderTexture.hpp>

#include "OpenGL2Shader.hpp"

namespace mordaren{

class OpenGL2ShaderPosTex : public morda::ShaderTexture, public OpenGL2Shader{
	GLint textureUniform;
public:
	OpenGL2ShaderPosTex();
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va, const morda::Texture2D& tex) override;
};

}
