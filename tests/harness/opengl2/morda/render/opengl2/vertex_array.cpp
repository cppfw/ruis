#include "vertex_array.hpp"

#include "util.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

using namespace morda::render_opengl2;

vertex_array::vertex_array(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode) :
		morda::vertex_array(std::move(buffers), std::move(indices), rendering_mode)
{

}
