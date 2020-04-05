#pragma once

#include <cstddef>

namespace morda{
	
class vertex_buffer{
public:
	const size_t size;
	
	vertex_buffer(size_t size) :
			size(size)
	{}

	virtual ~vertex_buffer()noexcept{}
};

}
