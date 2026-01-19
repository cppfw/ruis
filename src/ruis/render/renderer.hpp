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

#include "context.hpp"

namespace ruis::render {

class renderer : public std::enable_shared_from_this<renderer>
{
public:
	const utki::shared_ref<ruis::render::context> rendering_context;

	/**
	 * @brief Shorthand alias for render context.
	 * @return this->rendering_context.get().
	 */
	ruis::render::context& ctx() noexcept
	{
		return this->rendering_context.get();
	}

	/**
	 * @brief Shorthand alias for render context.
	 * @return this->rendering_context.get().
	 */
	ruis::render::context& ctx() const noexcept
	{
		return this->rendering_context.get();
	}

	const utki::shared_ref<const ruis::render::context::shaders> common_shaders;

	struct objects {
		objects(const ruis::render::context& rendering_context);

		utki::shared_ref<const vertex_array> empty_vertex_array;

		/**
		 * @brief Unit quad VBO.
		 * Vertices are in X-Y plane going counter-clockwise [(0, 0), (0, 1), (1, 1), (1, 0)].
		 */
		utki::shared_ref<const vertex_buffer> quad_01_vbo;

		utki::shared_ref<const index_buffer> quad_fan_indices;
		utki::shared_ref<const vertex_array> pos_quad_01_vao;
		utki::shared_ref<const vertex_array> pos_tex_quad_01_vao;
		utki::shared_ref<const texture_2d> white_texture;
	};

	// common objects may be shared between renderers
	const utki::shared_ref<const objects> common_objects;

	const objects& obj() const noexcept
	{
		return this->common_objects.get();
	}

	const ruis::render::context::shaders& shaders() const noexcept
	{
		return this->common_shaders.get();
	}

	renderer(
		utki::shared_ref<ruis::render::context> rendering_context, //
		utki::shared_ref<const ruis::render::context::shaders> common_shaders,
		utki::shared_ref<const objects> common_objects
	);

	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;

	renderer(renderer&&) = delete;
	renderer& operator=(renderer&&) = delete;

	virtual ~renderer() = default;

	/**
	 * @brief Render texture on a unit quad.
	 * @param matrix - transformation matrix to use for rendering.
	 * @param tex - texture to render.
	 */
	void render(
		const mat4& matrix, //
		const render::texture_2d& tex
	) const
	{
		this->render(
			matrix, //
			tex,
			this->obj().pos_tex_quad_01_vao.get()
		);
	}

	/**
	 * @brief Render a VAO with texture.
	 * @param matrix - transformation matrix to use for rendering.
	 * @param tex - texture to render.
	 * @param vao - vertex array to use for rendering.
	 */
	virtual void render(
		const mat4& matrix, //
		const render::texture_2d& tex,
		const render::vertex_array& vao
	) const
	{
		this->shaders().pos_tex->render(
			matrix, //
			vao,
			tex
		);
	}
};

} // namespace ruis::render
