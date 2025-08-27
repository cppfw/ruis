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

renderer::renderer(
	utki::shared_ref<ruis::render::context> rendering_context, //
	utki::shared_ref<const ruis::render::context::shaders> common_shaders,
	utki::shared_ref<const objects> common_objects
) :
	rendering_context(std::move(rendering_context)),
	common_shaders(std::move(common_shaders)),
	common_objects(std::move(common_objects))
{}

renderer::objects::objects(const ruis::render::context& rendering_context) :
	empty_vertex_array(rendering_context.make_vertex_array(
		{//
		 rendering_context.make_vertex_buffer(utki::span<const r4::vector2<float>>())
		},
		rendering_context.make_index_buffer(utki::span<const uint16_t>()),
		ruis::render::vertex_array::mode::triangle_strip
	)),
	quad_01_vbo(rendering_context.make_vertex_buffer(utki::make_span(std::array<vec2, 4>{
		vec2(0, 0), //
		vec2(0, 1),
		vec2(1, 1),
		vec2(1, 0)
	}))),
	quad_fan_indices(rendering_context.make_index_buffer(utki::make_span(std::array<uint16_t, 4>{0, 1, 2, 3}))),
	pos_quad_01_vao(rendering_context.make_vertex_array(
		{this->quad_01_vbo},
		this->quad_fan_indices,
		vertex_array::mode::triangle_fan
	)),
	pos_tex_quad_01_vao(rendering_context.make_vertex_array(
		{this->quad_01_vbo, this->quad_01_vbo},
		this->quad_fan_indices,
		vertex_array::mode::triangle_fan
	)),
	white_texture(rendering_context.make_texture_2d(
		[]() {
			// raster image 1 by 1 pixel
			rasterimage::image_variant imvar(
				{1, 1}, //
				rasterimage::format::rgba,
				rasterimage::depth::uint_8_bit
			);

			auto& im = imvar.get<rasterimage::format::rgba, rasterimage::depth::uint_8_bit>();
			constexpr auto opaque_white = std::remove_reference_t<decltype(im)>::pixel_type{0xff, 0xff, 0xff, 0xff};
			im[0][0] = opaque_white;
			return imvar;
		}(),
		{}
	))
{}
