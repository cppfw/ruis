#include "OpenGL2VertexArray.hpp"

#include "OpenGL2_util.hpp"
#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2IndexBuffer.hpp"

using namespace mordaren;

OpenGL2VertexArray::OpenGL2VertexArray(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode) :
		morda::vertex_array(std::move(buffers), std::move(indices), rendering_mode)
{

}
