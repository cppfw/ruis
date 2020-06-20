#include "OpenGLES2VertexArray.hpp"

#include "OpenGLES2_util.hpp"
#include "OpenGLES2VertexBuffer.hpp"
#include "OpenGLES2IndexBuffer.hpp"

using namespace morda::render_opengles2;

OpenGLES2VertexArray::OpenGLES2VertexArray(
		std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers,
		std::shared_ptr<morda::index_buffer> indices,
		mode rendering_mode
	) :
		morda::vertex_array(std::move(buffers), std::move(indices), rendering_mode)
{}
