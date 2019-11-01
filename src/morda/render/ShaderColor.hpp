#pragma once

#include <utki/Unique.hpp>

#include <r4/matrix4.hpp>

#include "VertexArray.hpp"

namespace morda{

class ShaderColor : public utki::Unique{
public:
	ShaderColor(){}
	
	ShaderColor(const ShaderColor&) = delete;
	ShaderColor& operator=(const ShaderColor&) = delete;
	
	virtual ~ShaderColor()noexcept{}
	
	virtual void render(const r4::mat4f &m, const VertexArray& va, r4::vec4f color)const = 0;

	void render(const r4::mat4f &m, const VertexArray& va, std::uint32_t color)const{
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
