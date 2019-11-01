#pragma once

#include <morda/render/ShaderColorTexture.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{	

class OpenGL2ShaderColorPosTex :
		public morda::ShaderColorTexture,
		public OpenGL2ShaderBase
{
	GLint colorUniform;
public:
	OpenGL2ShaderColorPosTex();
	
	OpenGL2ShaderColorPosTex(const OpenGL2ShaderColorPosTex&) = delete;
	OpenGL2ShaderColorPosTex& operator=(const OpenGL2ShaderColorPosTex&) = delete;
	
	void render(const r4::mat4f& m, const morda::VertexArray& va, r4::vec4f color, const morda::Texture2D& tex)const override;
};

}