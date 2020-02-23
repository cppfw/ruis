#pragma once

#include "Texture2D.hpp"

namespace morda{


class frame_buffer : public std::enable_shared_from_this<frame_buffer>{
protected:
	const std::shared_ptr<Texture2D> color;
public:
	frame_buffer(std::shared_ptr<Texture2D> color) :
			color(std::move(color))
	{}
	
	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;
	
	virtual ~frame_buffer()noexcept{}
private:

};


}
