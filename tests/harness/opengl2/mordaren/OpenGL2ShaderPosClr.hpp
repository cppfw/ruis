#pragma once

#include <morda/render/shader.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{

class OpenGL2ShaderPosClr :
		public morda::shader,
		public OpenGL2ShaderBase
{
public:
	OpenGL2ShaderPosClr();
	
	OpenGL2ShaderPosClr(const OpenGL2ShaderPosClr&) = delete;
	OpenGL2ShaderPosClr& operator=(const OpenGL2ShaderPosClr&) = delete;
	

	void render(const r4::mat4f& m, const morda::vertex_array& va) const override;
};

}
