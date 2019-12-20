#pragma once

#include <utki/shared.hpp>

#include "Texture2D.hpp"

namespace morda{


class FrameBuffer : public utki::shared{
protected:
	const std::shared_ptr<Texture2D> color;
public:
	FrameBuffer(std::shared_ptr<Texture2D> color) :
			color(color)
	{}
	
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	
private:

};


}
