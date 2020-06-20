#pragma once

#include <morda/render/vertex_array.hpp>

#include <GL/glew.h>

namespace morda{ namespace render_opengl2{

class vertex_array : public morda::vertex_array{
public:	
	vertex_array(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode);
	
	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	
private:

};

}}
