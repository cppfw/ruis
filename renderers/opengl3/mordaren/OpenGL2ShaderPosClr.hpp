#pragma once

#include <morda/render/ShaderPosClr.hpp>

#include "OpenGL2Shader.hpp"

class shader_pos_clr : public morda::ShaderPosClr, public OpenGL2Shader{
public:
	shader_pos_clr();
	
	shader_pos_clr(const shader_pos_clr&) = delete;
	shader_pos_clr& operator=(const shader_pos_clr&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va) const override;
};
