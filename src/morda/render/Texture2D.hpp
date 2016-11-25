#pragma once

#include "../config.hpp"

#include <utki/Shared.hpp>

namespace morda{
	

class Texture2D : virtual public utki::Shared{
	Vec2r dim_v;
	
public:
	Texture2D(Vec2r dim) :
			dim_v(dim)
	{}
	
	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	
	virtual ~Texture2D()noexcept{}

	const decltype(dim_v)& dim()const noexcept{
		return this->dim_v;
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
