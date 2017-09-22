#pragma once

#include <morda/render/ShaderColor.hpp>
#include "OpenGL2Shader.hpp"

namespace mordaren{	

class OpenGL2ShaderColorPos : public morda::ShaderColor, public OpenGL2Shader{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPos();
	
	OpenGL2ShaderColorPos(const OpenGL2ShaderColorPos&) = delete;
	OpenGL2ShaderColorPos& operator=(const OpenGL2ShaderColorPos&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va, kolme::Vec4f color) override;
};

}
