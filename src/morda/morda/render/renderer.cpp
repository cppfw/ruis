/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "renderer.hpp"

using namespace morda;

renderer::renderer(std::unique_ptr<render_factory> factory, const renderer::params& params) :
		factory(std::move(factory)),
		shader(this->factory->create_shaders()),
		quad_01_vbo(this->factory->create_vertex_buffer(utki::make_span(std::array<r4::vector2<float>, 4>({{
			r4::vector2<float>(0, 0), r4::vector2<float>(0, 1), r4::vector2<float>(1, 1), r4::vector2<float>(1, 0)
		}})))),
		quad_indices(this->factory->create_index_buffer(utki::make_span(std::array<uint16_t, 4>({{0, 1, 2, 3}})))),
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
