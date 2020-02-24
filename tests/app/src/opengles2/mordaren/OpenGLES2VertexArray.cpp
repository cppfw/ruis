#include "OpenGLES2VertexArray.hpp"

#include "OpenGLES2_util.hpp"
#include "OpenGLES2VertexBuffer.hpp"
#include "OpenGLES2IndexBuffer.hpp"

using namespace mordaren;

OpenGLES2VertexArray::OpenGLES2VertexArray(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode) :
		morda::vertex_array(std::move(buffers), std::move(indices), mode)
{

}
