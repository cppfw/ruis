#pragma once

#include "texture_2d.hpp"
#include "vertex_array.hpp"

namespace morda{

class texturing_shader{
public:
	texturing_shader(){}
	
	texturing_shader(const texturing_shader&) = delete;
	texturing_shader& operator=(const texturing_shader&) = delete;
	
	virtual ~texturing_shader()noexcept{}
	
	virtual void render(const r4::matrix4<float> &m, const morda::vertex_array& va, const texture_2d& tex)const = 0;
private:

};

}
