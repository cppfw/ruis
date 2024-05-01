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

#include "render_factory.hpp"

namespace ruis::render {

class renderer
{
public:
	const std::unique_ptr<render_factory> factory;

	const std::unique_ptr<render_factory::shaders> shader;

public:
	const utki::shared_ref<const vertex_array> empty_vertex_array;

	const utki::shared_ref<const vertex_buffer> quad_01_vbo;
	const utki::shared_ref<const index_buffer> quad_indices;

	const utki::shared_ref<const vertex_array> pos_quad_01_vao;

	const utki::shared_ref<const vertex_array> pos_tex_quad_01_vao;

protected:
	struct params {
		constexpr static auto default_max_texture_size = 2048;
		unsigned max_texture_size = default_max_texture_size;
		r4::matrix4<float> initial_matrix;
	};

	renderer(std::unique_ptr<render_factory> factory, const params& params);

private:
	std::shared_ptr<frame_buffer> cur_fb;

public:
	virtual ~renderer() = default;

	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;

	renderer(renderer&&) = delete;
	renderer& operator=(renderer&&) = delete;

	const unsigned max_texture_size;

	/**
	 * @brief Initial matrix to use for rendering.
	 * The initial coordinate system is right-handed.
	 * The initial matrix makes viewport edges to be: left = 0, right = 1, top = 0, bottom = 1.
	 */
	const r4::matrix4<float> initial_matrix;

	/**
	 * @brief Set current frame buffer.
	 * @param fb - frame buffer to set as the current one. If 'nullptr' then screen buffer is set as current frame
	 * buffer.
	 */
	void set_framebuffer(std::shared_ptr<frame_buffer> fb);

	/**
	 * @brief Get current frame buffer.
	 * @return Current frame buffer. If nullptr, then it is a screen buffer.
	 */
	const std::shared_ptr<frame_buffer>& get_framebuffer()
	{
		return this->cur_fb;
	}

	virtual void clear_framebuffer() = 0;

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

	virtual bool is_scissor_enabled() const = 0;

	virtual void set_scissor_enabled(bool enabled) = 0;

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

	virtual void set_blend_enabled(bool enable) = 0;

	/**
	 * @brief Blending factor type.
	 * Enumeration defines possible blending factor types.
	 */
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

protected:
	virtual void set_framebuffer_internal(frame_buffer* fb) = 0;
};

} // namespace ruis::render
