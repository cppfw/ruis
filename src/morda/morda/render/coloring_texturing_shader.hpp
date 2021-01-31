#pragma once

#include "texture_2d.hpp"
#include "vertex_array.hpp"

#include <r4/matrix.hpp>
#include <r4/vector.hpp>

namespace morda{

class coloring_texturing_shader{
public:
	coloring_texturing_shader(){}
	
	coloring_texturing_shader(const coloring_texturing_shader&) = delete;
	coloring_texturing_shader& operator=(const coloring_texturing_shader&) = delete;
	
	virtual ~coloring_texturing_shader()noexcept{}
	
	virtual void render(const r4::matrix4<float> &m, const morda::vertex_array& va, r4::vector4<float> color, const morda::texture_2d& tex)const = 0;
};

}
