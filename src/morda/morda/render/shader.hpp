#pragma once

#include <r4/matrix.hpp>

#include "vertex_array.hpp"

namespace morda{

class shader{
public:
	shader(){}
	
	virtual ~shader()noexcept{}
	
	virtual void render(const r4::matrix4<float>& m, const vertex_array& va)const = 0;
};

}
