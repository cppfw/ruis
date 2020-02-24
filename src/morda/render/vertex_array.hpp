#pragma once

#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

#include <utki/shared.hpp>

#include <vector>

namespace morda{

class vertex_array : virtual public utki::shared{
public:
	const std::vector<std::shared_ptr<vertex_buffer>> buffers;
	
	const std::shared_ptr<index_buffer> indices;
	
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
	
	vertex_array(decltype(buffers)&& buffers, std::shared_ptr<morda::index_buffer> indices, Mode_e mode);

};

}
