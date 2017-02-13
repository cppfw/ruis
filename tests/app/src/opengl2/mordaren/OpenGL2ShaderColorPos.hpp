#pragma once

#include <morda/render/ShaderColorPos.hpp>
#include "OpenGL2Shader.hpp"

namespace mordaren{	

class OpenGL2ShaderColorPos : public morda::ShaderColorPos, public OpenGL2Shader{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPos();
	
	OpenGL2ShaderColorPos(const OpenGL2ShaderColorPos&) = delete;
	OpenGL2ShaderColorPos& operator=(const OpenGL2ShaderColorPos&) = delete;
	

	void render(const kolme::Matr4f& m, kolme::Vec4f color, const morda::VertexArray& va) override;
};

}
