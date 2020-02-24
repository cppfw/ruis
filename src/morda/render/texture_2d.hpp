#pragma once

#include "../config.hpp"

#include <utki/shared.hpp>

namespace morda{
	

class texture_2d : virtual public utki::shared{
	Vec2r dims_v;
	
public:
	texture_2d(Vec2r dims) :
			dims_v(dims)
	{}
	
	texture_2d(const texture_2d&) = delete;
	texture_2d& operator=(const texture_2d&) = delete;
	
	virtual ~texture_2d()noexcept{}

	const decltype(dims_v)& dims()const noexcept{
		return this->dims_v;
	}

	enum class type{
		grey,
		grey_alpha,
		rgb,
		rgba
	};
	
	static unsigned bytes_per_pixel(texture_2d::type t);
};

}
