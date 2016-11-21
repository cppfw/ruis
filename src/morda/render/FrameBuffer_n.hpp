#pragma once

#include <utki/Shared.hpp>

#include "Texture2D_n.hpp"

namespace morda{


class FrameBuffer_n : public utki::Shared{
protected:
	const std::shared_ptr<Texture2D_n> color;
public:
	FrameBuffer_n(std::shared_ptr<Texture2D_n> color) :
			color(color)
	{}
	
	FrameBuffer_n(const FrameBuffer_n&) = delete;
	FrameBuffer_n& operator=(const FrameBuffer_n&) = delete;
	
private:

};


}
