#pragma once

#include "Texture2D.hpp"
#include "VertexArray.hpp"

#include <utki/Unique.hpp>

#include <r4/matrix4.hpp>
#include <r4/vector4.hpp>

namespace morda{

class coloring_texturing_shader{
public:
	coloring_texturing_shader(){}
	
	coloring_texturing_shader(const coloring_texturing_shader&) = delete;
	coloring_texturing_shader& operator=(const coloring_texturing_shader&) = delete;
	
	virtual ~coloring_texturing_shader()noexcept{}
	
	virtual void render(const r4::mat4f &m, const morda::VertexArray& va, r4::vec4f color, const morda::Texture2D& tex)const = 0;
};

}
