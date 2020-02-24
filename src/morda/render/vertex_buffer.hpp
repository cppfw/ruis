#pragma once

#include <utki/shared.hpp>

namespace morda{
	
class vertex_buffer : virtual public utki::shared{
public:
	const size_t size;
	
	vertex_buffer(size_t size) :
			size(size)
	{}
};

}
