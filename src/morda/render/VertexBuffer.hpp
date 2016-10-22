#pragma once

#include <utki/Shared.hpp>

namespace morda{
	
class VertexBuffer : virtual public utki::Shared{
	size_t size_v;
	
protected:
	
	VertexBuffer(size_t s) :
			size_v(s)
	{}
	
public:
	size_t size()const noexcept{
		return this->size_v;
	}
};

}
