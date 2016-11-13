#include "VertexArray.hpp"
#include "../Exc.hpp"

#include <utki/debug.hpp>

using namespace morda;

VertexArray::VertexArray(decltype(buffers)&& buffers, std::shared_ptr<morda::IndexBuffer> indices) :
		buffers(buffers),
		indices(std::move(indices))
{
	if(this->buffers.size() == 0){
		throw morda::Exc("no vertex buffers passed to vertex array");
	}
	ASSERT(this->buffers.front())
	auto s = this->buffers.front()->size;
	for(auto& b : this->buffers){
		if(b->size != s){
			throw morda::Exc("vertex buffers of different size passed in to vertex array, should all be of the same size");
		}
	}
}
