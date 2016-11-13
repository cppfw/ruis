#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include <utki/Shared.hpp>

#include <vector>

namespace morda{

class VertexArray : virtual public utki::Shared{
public:
	const std::vector<std::shared_ptr<VertexBuffer>> buffers;
	
	const std::shared_ptr<IndexBuffer> indices;
	
	VertexArray(decltype(buffers)&& buffers, std::shared_ptr<morda::IndexBuffer> indices);

};

}
