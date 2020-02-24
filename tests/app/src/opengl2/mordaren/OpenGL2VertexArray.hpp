#pragma once

#include <morda/render/vertex_array.hpp>

#include <GL/glew.h>

namespace mordaren{

class OpenGL2VertexArray : public morda::vertex_array{
public:	
	OpenGL2VertexArray(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode);
	
	OpenGL2VertexArray(const OpenGL2VertexArray&) = delete;
	OpenGL2VertexArray& operator=(const OpenGL2VertexArray&) = delete;

	
private:

};

}
