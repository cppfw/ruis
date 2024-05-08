#pragma once

#include <rasterimage/dimensioned.hpp>

namespace ruis::render {

class texture_stencil : public rasterimage::dimensioned
{
protected:
	texture_stencil(r4::vector2<uint32_t> dims) :
		rasterimage::dimensioned(dims)
	{}

public:
	texture_stencil(const texture_stencil&) = delete;
	texture_stencil& operator=(const texture_stencil&) = delete;

	texture_stencil(texture_stencil&&) = delete;
	texture_stencil& operator=(texture_stencil&&) = delete;

	virtual ~texture_stencil() = default;
};

} // namespace ruis::render
