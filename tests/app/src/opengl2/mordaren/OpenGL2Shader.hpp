#pragma once

#include <morda/render/Shader.hpp>

#include "OpenGL2ShaderBase.hpp"

namespace mordaren{

class OpenGL2Shader :
		public morda::Shader,
		public OpenGL2ShaderBase
{
public:
	OpenGL2Shader();
	
	OpenGL2Shader(const OpenGL2Shader&) = delete;
	OpenGL2Shader& operator=(const OpenGL2Shader&) = delete;
	

	void render(const kolme::Matr4f& m, const morda::VertexArray& va) const override;
};

}
