#pragma once

#include <utki/Shared.hpp>

namespace morda{
	
class VertexBuffer : virtual public utki::Shared{
public:
	const size_t size;
	
	VertexBuffer(size_t size) :
			size(size)
	{}

	
};

}
