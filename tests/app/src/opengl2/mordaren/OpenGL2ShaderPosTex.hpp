#pragma once

#include <morda/render/ShaderPosTex.hpp>

#include "OpenGL2Shader.hpp"

namespace mordaren{

class OpenGL2ShaderPosTex : public morda::ShaderPosTex, public OpenGL2Shader{
	GLint textureUniform;
public:
	OpenGL2ShaderPosTex();
	
	void render(const kolme::Matr4f& m, const morda::Texture2D& tex, const morda::VertexArray& va) override;
};

}
