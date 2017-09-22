#pragma once

#include <kolme/Matrix4.hpp>

#include <utki/Unique.hpp>

#include "VertexArray.hpp"

namespace morda{

class Shader : public utki::Unique{
public:
	Shader(){}
	
	virtual ~Shader()noexcept{}
	
	virtual void render(const kolme::Matr4f& m, const VertexArray& va)const = 0;
};

}
