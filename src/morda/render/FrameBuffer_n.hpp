#pragma once

#include <utki/Shared.hpp>

namespace morda{


class FrameBuffer_n : public utki::Shared{
public:
	FrameBuffer_n();
	
	FrameBuffer_n(const FrameBuffer_n&) = delete;
	FrameBuffer_n& operator=(const FrameBuffer_n&) = delete;
	
private:

};


}
