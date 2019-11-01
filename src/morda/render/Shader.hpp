#pragma once

#include <r4/matrix4.hpp>

#include <utki/Unique.hpp>

#include "VertexArray.hpp"

namespace morda{

class Shader : public utki::Unique{
public:
	Shader(){}
	
	virtual ~Shader()noexcept{}
	
	virtual void render(const r4::mat4f& m, const VertexArray& va)const = 0;
};

}
