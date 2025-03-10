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

#pragma once

#include <rasterimage/image_variant.hpp>
#include <utki/shared.hpp>

#include "shaders/coloring_shader.hpp"
#include "shaders/coloring_texturing_shader.hpp"
#include "shaders/shader.hpp"
#include "shaders/texturing_shader.hpp"

#include "frame_buffer.hpp"
#include "texture_cube.hpp"
#include "texture_depth.hpp"

namespace ruis::render {
class context : public std::enable_shared_from_this<context>
{
protected:
	utki::shared_ref<context> get_shared_ref()
	{
		return utki::make_shared_from(*this);
	}

public:
	context() = default;

	context(const context&) = delete;
	context& operator=(const context&) = delete;

	context(context&&) = delete;
	context& operator=(context&&) = delete;

	virtual ~context() = default;

	struct shaders {
		std::unique_ptr<texturing_shader> pos_tex;
		std::unique_ptr<coloring_shader> color_pos;
		std::unique_ptr<coloring_shader> color_pos_lum;
		std::unique_ptr<shader> pos_clr;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex_alpha;
	};

	virtual utki::shared_ref<shaders> create_shaders() = 0;

	struct texture_2d_parameters {
		texture_2d::filter min_filter = texture_2d::filter::nearest;
		texture_2d::filter mag_filter = texture_2d::filter::nearest;
		texture_2d::mipmap mipmap = texture_2d::mipmap::none;
	};

	virtual utki::shared_ref<texture_2d> create_texture_2d(
		rasterimage::format format,
		rasterimage::dimensioned::dimensions_type dims,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_2d> create_texture_2d(
		const rasterimage::image_variant& imvar,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_2d> create_texture_2d(
		rasterimage::image_variant&& imvar,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_depth> create_texture_depth(rasterimage::dimensioned::dimensions_type dims) = 0;

	virtual utki::shared_ref<texture_cube> create_texture_cube(
		rasterimage::image_variant&& positive_x,
		rasterimage::image_variant&& negative_x,
		rasterimage::image_variant&& positive_y,
		rasterimage::image_variant&& negative_y,
		rasterimage::image_variant&& positive_z,
		rasterimage::image_variant&& negative_z
	) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const float> vertices) = 0;

	virtual utki::shared_ref<index_buffer> create_index_buffer(utki::span<const uint16_t> indices) = 0;

	virtual utki::shared_ref<index_buffer> create_index_buffer(utki::span<const uint32_t> indices) = 0;

	virtual utki::shared_ref<vertex_array> create_vertex_array(
		std::vector<utki::shared_ref<const ruis::render::vertex_buffer>> buffers,
		utki::shared_ref<const ruis::render::index_buffer> indices,
		vertex_array::mode rendering_mode
	) = 0;

	virtual utki::shared_ref<frame_buffer> create_framebuffer( //
		std::shared_ptr<texture_2d> color,
		std::shared_ptr<texture_depth> depth,
		std::shared_ptr<texture_stencil> stencil
	) = 0;
};
} // namespace ruis::render