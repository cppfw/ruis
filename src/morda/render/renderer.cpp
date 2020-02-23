#include "renderer.hpp"

using namespace morda;


renderer::renderer(std::unique_ptr<RenderFactory> factory, const renderer::params& params) :
		factory(std::move(factory)),
		shader(this->factory->createShaders()),
		quad_01_vbo(this->factory->createVertexBuffer(utki::make_span(std::array<r4::vec2f, 4>({{
			r4::vec2f(0, 0), r4::vec2f(0, 1), r4::vec2f(1, 1), r4::vec2f(1, 0)
		}})))),
		quad_indices(this->factory->createIndexBuffer(utki::make_span(std::array<std::uint16_t, 4>({{0, 1, 2, 3}})))),
		pos_quad_01_vao(this->factory->createVertexArray({this->quad_01_vbo}, this->quad_indices, VertexArray::Mode_e::TRIANGLE_FAN)),
		pos_tex_quad_01_vao(this->factory->createVertexArray({this->quad_01_vbo, this->quad_01_vbo}, this->quad_indices, VertexArray::Mode_e::TRIANGLE_FAN)),
		max_texture_size(params.max_texture_size),
		initial_matrix(params.initial_matrix)
{
}



void renderer::set_framebuffer(std::shared_ptr<FrameBuffer> fb) {
	this->set_framebuffer_internal(fb.get());
	this->curFB = std::move(fb);
}
