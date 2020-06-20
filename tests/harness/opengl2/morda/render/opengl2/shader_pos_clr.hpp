#pragma once

#include <morda/render/shader.hpp>

#include "shader_base.hpp"

namespace morda{ namespace render_opengl2{

class shader_pos_clr :
		public morda::shader,
		public OpenGL2ShaderBase
{
public:
	shader_pos_clr();
	
	shader_pos_clr(const shader_pos_clr&) = delete;
	shader_pos_clr& operator=(const shader_pos_clr&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va) const override;
};

}}
