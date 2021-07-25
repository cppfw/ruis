#pragma once

#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

#include <vector>
#include <memory>

namespace morda{

class vertex_array{
public:
	const std::vector<std::shared_ptr<vertex_buffer>> buffers;
	
	const std::shared_ptr<index_buffer> indices;
	
	/**
	 * @brief Vertex data rendering mode.
	 * Enumeration defining how to interpret vertex data when rendering.
	 */
	enum class mode{
		// NOTE: do not change order!!!
		
		triangles,
		triangle_fan,
		line_loop,
		triangle_strip
	};
	
	const mode rendering_mode;
	
	vertex_array(decltype(buffers)&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode);

	virtual ~vertex_array(){}
};

}
