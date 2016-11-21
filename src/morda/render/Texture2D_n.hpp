#pragma once

#include "../config.hpp"

#include <utki/Shared.hpp>

namespace morda{
	

class Texture2D_n : virtual public utki::Shared{
	Vec2r dim_v;
	
public:
	Texture2D_n(Vec2r dim) :
			dim_v(dim)
	{}
	
	Texture2D_n(const Texture2D_n&) = delete;
	Texture2D_n& operator=(const Texture2D_n&) = delete;
	
	virtual ~Texture2D_n()noexcept{}

	const decltype(dim_v)& dim()const noexcept{
		return this->dim_v;
	}

	enum class TexType_e{
		GREY,
		GREYA,
		RGB,
		RGBA
	};
	
	static unsigned bytesPerPixel(Texture2D_n::TexType_e t);
};

}
