/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <memory>
#include <vector>

#include <r4/vector.hpp>
#include <rasterimage/dimensioned.hpp>
#include <rasterimage/image_variant.hpp>
#include <utki/shared.hpp>
#include <utki/span.hpp>

#include "shaders/coloring_shader.hpp"
#include "shaders/coloring_texturing_shader.hpp"
#include "shaders/shader.hpp"
#include "shaders/texturing_shader.hpp"

#include "index_buffer.hpp"
#include "native_window.hpp"
#include "texture_2d.hpp"
#include "texture_cube.hpp"
#include "texture_depth.hpp"
#include "texture_stencil.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

namespace ruis::render {
class context;

/**
 * @brief Render object factory.
 *
 * Owns the shared (headless) rendering context under the hood and is the only place
 * that creates the shareable rendering objects (textures, vertex/index buffers,
 * vertex arrays, shaders). Objects created by the factory hold a reference to the
 * factory (not to a per-window render context), so they can be safely used by any
 * render context in the process.
 *
 * A per-window @ref context is injected with a reference to the factory and is only
 * used for state control (scissor, viewport, blending, depth, vsync, current frame
 * buffer) and for creating per-window @ref frame_buffer objects via its
 * @ref context::make_framebuffer().
 */
class factory : public std::enable_shared_from_this<factory>
{
	// ruis::render::context calls the protected bind() when the last render
	// context is destroyed, to make the shared rendering context current
	friend class context;

public:
	virtual ~factory() = default;

protected:
	factory() = default;

	utki::shared_ref<const factory> get_shared_ref() const
	{
		return utki::shared_ref<const factory>(this->shared_from_this());
	}

	/**
	 * @brief Make this factory's shared rendering context current.
	 *
	 * The default (empty) implementation is for renderers that have no notion
	 * of a current context (e.g. Vulkan). GL-based factories override it to
	 * make their shared (headless) GL context current. Called by
	 * ruis::render::context when the last render context is destroyed, so
	 * that the shared render objects created by the factory can be destroyed
	 * with a valid current context.
	 */
	virtual void bind() const {}

public:
	factory(const factory&) = delete;
	factory& operator=(const factory&) = delete;

	factory(factory&&) = delete;
	factory& operator=(factory&&) = delete;

	/**
	 * @brief Rendering shaders.
	 * The specific shader objects are created by the concrete renderer's make_shaders().
	 */
	struct shaders {
		std::unique_ptr<const texturing_shader> pos_tex;
		std::unique_ptr<const coloring_shader> color_pos;
		std::unique_ptr<const coloring_shader> color_pos_lum;
		std::unique_ptr<const shader> pos_clr;
		std::unique_ptr<const coloring_texturing_shader> color_pos_tex;
		std::unique_ptr<const coloring_texturing_shader> color_pos_tex_alpha;
	};

	/**
	 * @brief Create the common rendering shaders.
	 */
	virtual utki::shared_ref<shaders> make_shaders() const = 0;

	struct texture_2d_parameters {
		texture_2d::filter min_filter = texture_2d::filter::nearest;
		texture_2d::filter mag_filter = texture_2d::filter::nearest;
		texture_2d::mipmap mipmap = texture_2d::mipmap::none;
	};

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		rasterimage::format format,
		rasterimage::dimensioned::dimensions_type dims,
		texture_2d_parameters params
	) const = 0;

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		const rasterimage::image_variant& imvar,
		texture_2d_parameters params
	) const = 0;

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		rasterimage::image_variant&& imvar,
		texture_2d_parameters params
	) const = 0;

	virtual utki::shared_ref<texture_depth> make_texture_depth(rasterimage::dimensioned::dimensions_type dims
	) const = 0;

	virtual utki::shared_ref<texture_cube> make_texture_cube(
		rasterimage::image_variant&& positive_x,
		rasterimage::image_variant&& negative_x,
		rasterimage::image_variant&& positive_y,
		rasterimage::image_variant&& negative_y,
		rasterimage::image_variant&& positive_z,
		rasterimage::image_variant&& negative_z
	) const = 0;

	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector4<float>> vertices) const = 0;
	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector3<float>> vertices) const = 0;
	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector2<float>> vertices) const = 0;
	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const float> vertices) const = 0;

	virtual utki::shared_ref<index_buffer> make_index_buffer(utki::span<const uint16_t> indices) const = 0;
	virtual utki::shared_ref<index_buffer> make_index_buffer(utki::span<const uint32_t> indices) const = 0;

	virtual utki::shared_ref<vertex_array> make_vertex_array(
		std::vector<utki::shared_ref<const ruis::render::vertex_buffer>> buffers,
		utki::shared_ref<const ruis::render::index_buffer> indices,
		vertex_array::mode rendering_mode
	) const = 0;
};

} // namespace ruis::render
