#pragma once

#include <utki/shared.hpp>

namespace morda{
	
class VertexBuffer : virtual public utki::shared{
public:
	const size_t size;
	
	VertexBuffer(size_t size) :
			size(size)
	{}
};

}
