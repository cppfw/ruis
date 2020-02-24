#pragma once

#include <r4/matrix4.hpp>

#include "VertexArray.hpp"

namespace morda{

class shader{
public:
	shader(){}
	
	virtual ~shader()noexcept{}
	
	virtual void render(const r4::mat4f& m, const VertexArray& va)const = 0;
};

}
