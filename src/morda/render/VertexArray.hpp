#pragma once

#include "VertexBuffer.hpp"

#include <utki/Shared.hpp>

#include <vector>

namespace morda{

class VertexArray : virtual public utki::Shared{
	std::vector<std::shared_ptr<VertexBuffer>> buffers;
public:
	
	
};

}
