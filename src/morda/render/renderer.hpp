/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

namespace morda {

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
		unsigned max_texture_size = 2048;
		r4::matrix4<float> initial_matrix = r4::matrix4<float>().set_identity();
	};

	renderer(std::unique_ptr<render_factory> factory, const params& params);

	virtual ~renderer() = default;

private:
	std::shared_ptr<frame_buffer> cur_fb;

public:
	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;

	const unsigned max_texture_size;

	/**
	 * @brief Initial matrix to use for rendering.
	 * This is the matrix which makes screen edges to be: left = -1, right = 1, top = 1, bottom = -1.
	 */
	const r4::matrix4<float> initial_matrix;

	/**
	 * @brief Set current framebuffer.
	 * @param fb - framebuffer to set as the current one. If 'nullptr' then screen buffer is set as current frame
	 * buffer.
	 */
	void set_framebuffer(std::shared_ptr<frame_buffer> fb);

	virtual void clear_framebuffer() = 0;

	virtual bool is_scissor_enabled() const = 0;

	virtual void set_scissor_enabled(bool enabled) = 0;

	virtual r4::rectangle<int> get_scissor() const = 0;

	virtual void set_scissor(r4::rectangle<int> r) = 0;

	virtual r4::rectangle<int> get_viewport() const = 0;

	virtual void set_viewport(r4::rectangle<int> r) = 0;

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
		src_alpha_saturate
	};

	virtual void set_blend_func(
		blend_factor src_color,
		blend_factor dst_color,
		blend_factor src_alpha,
		blend_factor dst_alpha
	) = 0;

protected:
	virtual void set_framebuffer_internal(frame_buffer* fb) = 0;
};

} // namespace morda
