#pragma once

#include "../config.hpp"

#include <utki/shared.hpp>

namespace morda{
	

class Texture2D : virtual public utki::shared{
	Vec2r dims_v;
	
public:
	Texture2D(Vec2r dims) :
			dims_v(dims)
	{}
	
	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	
	virtual ~Texture2D()noexcept{}

	const decltype(dims_v)& dims()const noexcept{
		return this->dims_v;
	}

	enum class TexType_e{
		GREY,
		GREYA,
		RGB,
		RGBA
	};
	
	static unsigned bytesPerPixel(Texture2D::TexType_e t);
};

}
