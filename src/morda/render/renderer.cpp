#include "renderer.hpp"

using namespace morda;


renderer::renderer(std::unique_ptr<render_factory> factory, const renderer::params& params) :
		factory(std::move(factory)),
		shader(this->factory->create_shaders()),
		quad_01_vbo(this->factory->create_vertex_buffer(utki::make_span(std::array<r4::vec2f, 4>({{
			r4::vec2f(0, 0), r4::vec2f(0, 1), r4::vec2f(1, 1), r4::vec2f(1, 0)
		}})))),
		quad_indices(this->factory->create_index_buffer(utki::make_span(std::array<std::uint16_t, 4>({{0, 1, 2, 3}})))),
		pos_quad_01_vao(this->factory->create_vertex_array({this->quad_01_vbo}, this->quad_indices, vertex_array::mode::triangle_fan)),
		pos_tex_quad_01_vao(this->factory->create_vertex_array({this->quad_01_vbo, this->quad_01_vbo}, this->quad_indices, vertex_array::mode::triangle_fan)),
		max_texture_size(params.max_texture_size),
		initial_matrix(params.initial_matrix)
{
}



void renderer::set_framebuffer(std::shared_ptr<frame_buffer> fb){
	this->set_framebuffer_internal(fb.get());
	this->curFB = std::move(fb);
}

