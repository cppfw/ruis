/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include <rasterimage/image_variant.hpp>

using namespace ruis::render;

renderer::renderer(utki::shared_ref<ruis::render::context> render_context) :
	render_context(std::move(render_context)),
	common_objects(utki::make_shared<objects>([this]() -> objects {
		objects self{
			.shaders = this->render_context.get().create_shaders(),
			.empty_vertex_array = this->render_context.get().create_vertex_array(
				{this->render_context.get().create_vertex_buffer(utki::span<const r4::vector2<float>>())},
				this->render_context.get().create_index_buffer(utki::span<const uint16_t>()),
				ruis::render::vertex_array::mode::triangle_strip
			),
			.quad_01_vbo = this->render_context.get().create_vertex_buffer(
				utki::make_span(
					std::array<r4::vector2<float>, 4>(
						{{r4::vector2<float>(0, 0),
						  r4::vector2<float>(0, 1),
						  r4::vector2<float>(1, 1),
						  r4::vector2<float>(1, 0)}}
					)
				)
			),
			.quad_indices = this->render_context.get().create_index_buffer(
				utki::make_span(std::array<uint16_t, 4>({{0, 1, 2, 3}}
                  ))
			),
			.pos_quad_01_vao =
				this->render_context.get()
					.create_vertex_array({self.quad_01_vbo},
                  self.quad_indices, vertex_array::mode::triangle_fan),
			.pos_tex_quad_01_vao = this->render_context.get().create_vertex_array(
				{self.quad_01_vbo, self.quad_01_vbo},
				self.quad_indices,
				vertex_array::mode::triangle_fan
			),
			.white_texture = this->render_context.get().create_texture_2d(
				[]() {
					// raster image 1 by 1 pixel
					rasterimage::image_variant imvar(
						{1, 1}, //
						rasterimage::format::rgba,
						rasterimage::depth::uint_8_bit
					);

					auto& im = imvar.get<rasterimage::format::rgba, rasterimage::depth::uint_8_bit>();
					constexpr auto opaque_white =
						std::remove_reference_t<decltype(im)>::pixel_type{0xff, 0xff, 0xff, 0xff};
					im[0][0] = opaque_white;
					return imvar;
																					 }
				  (),
				{}
			)
		};
		return self;
	}()))
{}
