#pragma once

#include <kolme/Matrix4.hpp>

#include "Shader_n.hpp"
#include "VertexArray.hpp"

namespace morda{

class ShaderColorPos : public Shader_n{
public:
	ShaderColorPos();
	
	ShaderColorPos(const ShaderColorPos&) = delete;
	ShaderColorPos& operator=(const ShaderColorPos&) = delete;
	
	virtual void render(const kolme::Matr4f &m, const VertexArray& va, VertexArray::Mode_e mode) = 0;

};

}
