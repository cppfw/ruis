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
#include "native_window.hpp"
#include "texture_cube.hpp"
#include "texture_depth.hpp"

namespace ruis::render {
class context : public std::enable_shared_from_this<context>
{
	friend class frame_buffer;

protected:
	utki::shared_ref<context> get_shared_ref()
	{
		return utki::make_shared_from(*this);
	}

private:
	std::weak_ptr<frame_buffer> cur_fb;

public:
	const utki::shared_ref<ruis::native_window> native_window;

	struct parameters {
		r4::matrix4<float> initial_matrix;
	};

	/**
	 * @brief Initial matrix to use for rendering.
	 * The initial coordinate system is right-handed.
	 * The initial matrix makes viewport edges to be: left = 0, right = 1, top = 0, bottom = 1.
	 */
	const r4::matrix4<float> initial_matrix;

	context(
		utki::shared_ref<ruis::native_window>, //
		parameters params
	);

	context(const context&) = delete;
	context& operator=(const context&) = delete;

	context(context&&) = delete;
	context& operator=(context&&) = delete;

	virtual ~context() = default;

	bool is_current() const noexcept
	{
		// TODO: actually check that the context is current
		return true;
	}

	/**
	 * @brief Execute procedure with this context made current.
	 * @param proc - procedure to execute.
	 */
	void apply(std::function<void()> proc);

	// ===============================
	// ====== factory functions ======

	struct shaders {
		// TODO: make unique_ref
		std::unique_ptr<const texturing_shader> pos_tex;
		std::unique_ptr<const coloring_shader> color_pos;
		std::unique_ptr<const coloring_shader> color_pos_lum;
		std::unique_ptr<const shader> pos_clr;
		std::unique_ptr<const coloring_texturing_shader> color_pos_tex;
		std::unique_ptr<const coloring_texturing_shader> color_pos_tex_alpha;
	};

	virtual utki::shared_ref<shaders> make_shaders() = 0;

	struct texture_2d_parameters {
		texture_2d::filter min_filter = texture_2d::filter::nearest;
		texture_2d::filter mag_filter = texture_2d::filter::nearest;
		texture_2d::mipmap mipmap = texture_2d::mipmap::none;
	};

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		rasterimage::format format,
		rasterimage::dimensioned::dimensions_type dims,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		const rasterimage::image_variant& imvar,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_2d> make_texture_2d(
		rasterimage::image_variant&& imvar,
		texture_2d_parameters params
	) = 0;

	virtual utki::shared_ref<texture_depth> make_texture_depth(rasterimage::dimensioned::dimensions_type dims) = 0;

	virtual utki::shared_ref<texture_cube> make_texture_cube(
		rasterimage::image_variant&& positive_x,
		rasterimage::image_variant&& negative_x,
		rasterimage::image_variant&& positive_y,
		rasterimage::image_variant&& negative_y,
		rasterimage::image_variant&& positive_z,
		rasterimage::image_variant&& negative_z
	) = 0;

	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector4<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector3<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const r4::vector2<float>> vertices) = 0;

	virtual utki::shared_ref<vertex_buffer> make_vertex_buffer(utki::span<const float> vertices) = 0;

	virtual utki::shared_ref<index_buffer> make_index_buffer(utki::span<const uint16_t> indices) = 0;

	virtual utki::shared_ref<index_buffer> make_index_buffer(utki::span<const uint32_t> indices) = 0;

	virtual utki::shared_ref<vertex_array> make_vertex_array(
		std::vector<utki::shared_ref<const ruis::render::vertex_buffer>> buffers,
		utki::shared_ref<const ruis::render::index_buffer> indices,
		vertex_array::mode rendering_mode
	) = 0;

	virtual utki::shared_ref<frame_buffer> make_framebuffer( //
		std::shared_ptr<texture_2d> color,
		std::shared_ptr<texture_depth> depth,
		std::shared_ptr<texture_stencil> stencil
	) = 0;

	// =====================================
	// ====== state control functions ======

private:
	/**
	 * @brief Set current frame buffer.
	 * @param fb - frame buffer to set as the current one.
	 *             If 'nullptr' then screen buffer is set as current frame buffer.
	 */
	void set_framebuffer(frame_buffer* fb);

	/**
	 * @brief Get current frame buffer.
	 * @return Current frame buffer. If nullptr, then it is a screen buffer.
	 */
	std::shared_ptr<frame_buffer> get_framebuffer()
	{
		return this->cur_fb.lock();
	}

public:
	/**
	 * @brief Clear color buffer of the current framebuffer.
	 * The color buffer is filled with (0, 0, 0, 0) color values.
	 */
	virtual void clear_framebuffer_color() = 0;

	/**
	 * @brief Clear depth buffer of the current framebuffer.
	 * The depth buffer is filled with values of 1.
	 */
	virtual void clear_framebuffer_depth() = 0;

	/**
	 * @brief Clear stencil buffer of the current framebuffer.
	 * The stencil buffer is filled with values of 0.
	 */
	virtual void clear_framebuffer_stencil() = 0;

	/**
	 * @brief Get window coordinates of a point in renderer's clipping coordinates.
	 * Renderer's clipping coordinates of a point are coordinates after all matrix transformations
	 * and perspective division.
	 * The window coordinate system is renderer-specific. It can be top-bottom or bottom-top.
	 * For example, OpenGL/ES renderer has bottom-top coordinate system, i.e. origin is
	 * at the bottom left corner of the window and y-axis goes up. So, the resulting position of a point
	 * should be interpreted in context of the specific renderer.
	 * @return Window coordinates of the point.
	 */
	virtual r4::vector2<uint32_t> to_window_coords(ruis::vec2 point) const = 0;

	/**
	 * @brief Check if scissor test is enabled.
	 * @return true if scissor test is enabled.
	 * @return false otherwise.
	 */
	virtual bool is_scissor_enabled() const noexcept = 0;

	/**
	 * @brief Enable/disable scissor test.
	 * @param enable - if true the scissor test will be enabled. Otherwise, it will be disabled.
	 */
	virtual void enable_scissor(bool enable) = 0;

	/**
	 * @brief Get scissor rectangle.
	 * Get scissor rectangle in application window coordinates.
	 * TODO:
	 *
	 * @return Current scissor rectangle.
	 */
	virtual r4::rectangle<uint32_t> get_scissor() const = 0;

	/**
	 * @brief Set scissor rectangle.
	 * Set scissor rectangle in application window coordinates.
	 * TODO:
	 * @param r - new scissor rectangle.
	 */
	virtual void set_scissor(r4::rectangle<uint32_t> r) = 0;

	/**
	 * @brief Get current rendering viewport within application window.
	 * Get the rendering viewport rectangle in application window coordinates.
	 * The window coordinate system is renderer-specific. It can be top-bottom or bottom-top.
	 * For example, OpenGL/ES renderer has bottom-top coordinate system, i.e. origin is
	 * at the bottom left corner of the window and y-axis goes up. So, the viewport's position
	 * should be interpreted in context of the specific renderer.
	 * The viewport's dimensions are renderer-agnostic.
	 * @return Current rendering viewport rectangle.
	 */
	// TODO: int -> unsigned?
	virtual r4::rectangle<uint32_t> get_viewport() const = 0;

	/**
	 * @brief Set rendering viewport within application window.
	 * Since application window's coordinate system is renderer-specific
	 * (see description of get_viewport() for details), the meaning of the viewport rectangle's
	 * position is also renderer-specific. Though, the position of (0, 0) in combination with
	 * framebuffer's dimensions can safely be used regardless of the renderer to set the viewport
	 * covering the whole framebuffer.
	 * @param r - new viewport rectangle.
	 */
	virtual void set_viewport(r4::rectangle<uint32_t> r) = 0;

	virtual void enable_blend(bool enable) = 0;

	/**
	 * @brief Blending factor type.
	 * Enumeration defines possible blending factor types.
	 */
	// TODO: move out to ruis::render namespace
	enum class blend_factor {
		// WARNING: do not change order

		zero,
		one,
		src_color,
		one_minus_src_color,
		dst_color,
		one_minus_dst_color,
		src_alpha,
		one_minus_src_alpha,
		dst_alpha,
		one_minus_dst_alpha,
		constant_color,
		one_minus_constant_color,
		constant_alpha,
		one_minus_constant_alpha,
		src_alpha_saturate,

		enum_size
	};

	virtual void set_blend_func(
		blend_factor src_color,
		blend_factor dst_color,
		blend_factor src_alpha,
		blend_factor dst_alpha
	) = 0;

	/**
	 * @brief Set simple alpha blending.
	 * Enables and set simple alpha blending on the rendering context.
	 * Blend factors are SRC_ALPHA and ONE_MINUS_SRC_ALPHA for source and destination RGB color components respectively.
	 * And, ONE and ONE_MINUS_SRC_ALPHA for source and destination alpha components respectively.
	 */
	void set_simple_alpha_blending();

	/**
	 * @brief Check if depth test is enabled.
	 * @return true if depth test is enabled.
	 * @return false otherwise.
	 */
	virtual bool is_depth_enabled() const noexcept = 0;

	/**
	 * @brief Enable/disable depth test.
	 *
	 * @param enable - if true the depth test will be enabled. Otherwise, it will be disabled.
	 */
	virtual void enable_depth(bool enable) = 0;

protected:
	virtual void set_framebuffer_internal(frame_buffer* fb) = 0;
};
} // namespace ruis::render