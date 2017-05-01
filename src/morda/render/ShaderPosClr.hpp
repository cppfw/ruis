#pragma once

#include <kolme/Matrix4.hpp>

#include "Shader.hpp"
#include "VertexArray.hpp"

namespace morda{

class ShaderPosClr : public Shader{
public:
	ShaderPosClr(){}
	
	virtual void render(const kolme::Matr4f& m, const VertexArray& va)const = 0;
};

}
