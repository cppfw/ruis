#pragma once

#include <r4/matrix4.hpp>

#include "vertex_array.hpp"

namespace morda{

class coloring_shader{
public:
	coloring_shader(){}
	
	coloring_shader(const coloring_shader&) = delete;
	coloring_shader& operator=(const coloring_shader&) = delete;
	
	virtual ~coloring_shader()noexcept{}
	
	virtual void render(const r4::mat4f &m, const vertex_array& va, r4::vec4f color)const = 0;

	void render(const r4::mat4f &m, const vertex_array& va, std::uint32_t color)const{
		this->render(
				m,
				va,
				r4::vec4f(
						float(color & 0xff) / float(0xff),
						float((color >> 8) & 0xff) / float(0xff),
						float((color >> 16) & 0xff) / float(0xff),
						float((color >> 24) & 0xff) / float(0xff)
					)
			);
	}
};

}
