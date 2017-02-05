#pragma once

#include <morda/render/ShaderPosClr.hpp>

#include "OpenGL2Shader.hpp"

namespace mordaren{

class OpenGL2ShaderPosClr : public morda::ShaderPosClr, public OpenGL2Shader{
public:
	OpenGL2ShaderPosClr();
	
	OpenGL2ShaderPosClr(const OpenGL2ShaderPosClr&) = delete;
	OpenGL2ShaderPosClr& operator=(const OpenGL2ShaderPosClr&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va) const override;
};

}
