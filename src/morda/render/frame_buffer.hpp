#pragma once

#include "texture_2d.hpp"

namespace morda{


class frame_buffer : public std::enable_shared_from_this<frame_buffer>{
protected:
	const std::shared_ptr<texture_2d> color;
public:
	frame_buffer(std::shared_ptr<texture_2d> color) :
			color(std::move(color))
	{}
	
	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;
	
	virtual ~frame_buffer()noexcept{}
private:

};


}
