#pragma once

#include <morda/render/ShaderColorPosTex.hpp>

#include "OpenGL2Shader.hpp"

namespace mordaren{	

class OpenGL2ShaderColorPosTex : public morda::ShaderColorPosTex, public OpenGL2Shader{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosTex();
	
	OpenGL2ShaderColorPosTex(const OpenGL2ShaderColorPosTex&) = delete;
	OpenGL2ShaderColorPosTex& operator=(const OpenGL2ShaderColorPosTex&) = delete;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D& tex, kolme::Vec4f color, const morda::VertexArray& va) override;
};

}