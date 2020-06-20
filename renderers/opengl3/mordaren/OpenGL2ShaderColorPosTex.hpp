#pragma once

#include <morda/render/ShaderColorPosTex.hpp>

#include "OpenGL2Shader.hpp"

class shader_color_pos_tex : public morda::ShaderColorPosTex, public OpenGL2Shader{
	GLint colorUniform;
public:
	shader_color_pos_tex();
	
	shader_color_pos_tex(const shader_color_pos_tex&) = delete;
	shader_color_pos_tex& operator=(const shader_color_pos_tex&) = delete;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D& tex, kolme::Vec4f color, const morda::VertexArray& va) override;
};
