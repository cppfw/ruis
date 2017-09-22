#pragma once

#include <morda/render/Shader.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{

class OpenGL2ShaderPosClr :
		public morda::Shader,
		public OpenGL2ShaderBase
{
public:
	OpenGL2ShaderPosClr();
	
	OpenGL2ShaderPosClr(const OpenGL2ShaderPosClr&) = delete;
	OpenGL2ShaderPosClr& operator=(const OpenGL2ShaderPosClr&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va) const override;
};

}
