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
	
	/**
	 * @brief Vertex data rendering mode.
	 * Enumeration defining how to interpret vertex data when rendering.
	 */
	enum class Mode_e{
		//NOTE: do not change order!!!
		
		TRIANGLES,
		TRIANGLE_FAN,
		LINE_LOOP,
		TRIANGLE_STRIP,
		
		ENUM_SIZE
	};
	
	const Mode_e mode;
	
	VertexArray(decltype(buffers)&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);

};

}
