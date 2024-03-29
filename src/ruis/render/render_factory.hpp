/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <vector>

#include <r4/vector.hpp>
#include <rasterimage/image_variant.hpp>
#include <utki/shared_ref.hpp>
#include <utki/span.hpp>

#include "coloring_shader.hpp"
#include "coloring_texturing_shader.hpp"
#include "frame_buffer.hpp"
#include "index_buffer.hpp"
#include "shader.hpp"
#include "texture_2d.hpp"
#include "texturing_shader.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

namespace ruis {

class render_factory
{
protected:
	render_factory() = default;

public:
	render_factory(const render_factory&) = delete;
	render_factory& operator=(const render_factory&) = delete;

	render_factory(render_factory&&) = delete;
	render_factory& operator=(render_factory&&) = delete;

	virtual ~render_factory() = default;

	virtual utki::shared_ref<texture_2d> create_texture_2d(
		rasterimage::format format,
		rasterimage::dimensioned::dimensions_type dims
	) = 0;

	virtual utki::shared_ref<texture_2d> create_texture_2d(const rasterimage::image_variant& imvar) = 0;

	// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
	virtual utki::shared_ref<texture_2d> create_texture_2d(rasterimage::image_variant&& imvar)
	{
		return this->create_texture_2d(imvar);
	}

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> create_vertex_buffer(utki::span<const float> vertices) = 0;

	virtual utki::shared_ref<index_buffer> create_index_buffer(utki::span<const uint16_t> indices) = 0;

	virtual utki::shared_ref<vertex_array> create_vertex_array(
		std::vector<utki::shared_ref<const ruis::vertex_buffer>> buffers,
		const utki::shared_ref<const ruis::index_buffer>& indices,
		vertex_array::mode rendering_mode
	) = 0;

	struct shaders {
		std::unique_ptr<texturing_shader> pos_tex;
		std::unique_ptr<coloring_shader> color_pos;
		std::unique_ptr<coloring_shader> color_pos_lum;
		std::unique_ptr<shader> pos_clr;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex_alpha;
	};

	virtual std::unique_ptr<shaders> create_shaders() = 0;

	virtual utki::shared_ref<frame_buffer> create_framebuffer(const utki::shared_ref<texture_2d>& color) = 0;
};

} // namespace ruis
