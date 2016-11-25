#pragma once

#include "Shader.hpp"
#include "Texture2D.hpp"
#include "VertexArray.hpp"

#include <kolme/Matrix4.hpp>
#include <kolme/Vector4.hpp>

namespace morda{

class ShaderColorPosTex : public Shader_n{
public:
	ShaderColorPosTex(){}
	
	ShaderColorPosTex(const ShaderColorPosTex&) = delete;
	ShaderColorPosTex& operator=(const ShaderColorPosTex&) = delete;
	
	virtual void render(const kolme::Matr4f &m, const morda::Texture2D& tex, kolme::Vec4f color, const morda::VertexArray& va) = 0;
};

}
