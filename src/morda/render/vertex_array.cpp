#include "vertex_array.hpp"

#include <utki/debug.hpp>

using namespace morda;

vertex_array::vertex_array(decltype(buffers)&& buffers, std::shared_ptr<morda::index_buffer> indices, Mode_e mode) :
		buffers(buffers),
		indices(std::move(indices)),
		mode(mode)
{
	if(this->buffers.empty()){
		throw std::invalid_argument("no vertex buffers passed to vertex array");
	}
	ASSERT(this->buffers.front())
	auto s = this->buffers.front()->size;
	for(auto& b : this->buffers){
		if(b->size != s){
			throw std::invalid_argument("vertex buffers of different size passed in to vertex array, should all be of the same size");
		}
	}
}
