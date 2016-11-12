#pragma once

#include "VertexBuffer.hpp"

#include <utki/Shared.hpp>

#include <vector>

namespace morda{

class VertexArray : virtual public utki::Shared{
	std::vector<std::shared_ptr<VertexBuffer>> buffers;
public:
	
	VertexArray(decltype(buffers)&& buffers) :
			buffers(buffers)
	{}
	
	VertexBuffer& get(size_t i){
		return *this->buffers[i];
	}
};

}
