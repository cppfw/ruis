#pragma once

#include <utki/Unique.hpp>

#include <kolme/Matrix4.hpp>

#include "VertexArray.hpp"

namespace morda{

class ShaderColor : public utki::Unique{
public:
	ShaderColor(){}
	
	ShaderColor(const ShaderColor&) = delete;
	ShaderColor& operator=(const ShaderColor&) = delete;
	
	virtual ~ShaderColor()noexcept{}
	
	virtual void render(const kolme::Matr4f &m, const VertexArray& va, kolme::Vec4f color) = 0;

	void render(const kolme::Matr4f &m, const VertexArray& va, std::uint32_t color){
		this->render(
				m,
				va,
				kolme::Vec4f(
						float(color & 0xff) / float(0xff),
						float((color >> 8) & 0xff) / float(0xff),
						float((color >> 16) & 0xff) / float(0xff),
						float((color >> 24) & 0xff) / float(0xff)
					)
			);
	}
};

}
