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

#include "../render/renderer.hpp"
#include "../util/color.hpp"

#include "path.hpp"

namespace ruis::paint {

/**
 * @brief A vertex array object which renders a stroked path.
 * A path_vao holds the core (fill) and border (stroke) vertex arrays used to
 * draw a path. The border is used for the anti-aliasing effect.
 */
class path_vao
{
	utki::shared_ref<const ruis::render::renderer> renderer;

	utki::shared_ref<const ruis::render::vertex_array> core;
	utki::shared_ref<const ruis::render::vertex_array> border;

public:
	/**
	 * @brief Constructor.
	 * @param r - renderer to use.
	 */
	path_vao(utki::shared_ref<const ruis::render::renderer> r);

	path_vao(const path_vao&) = delete;
	path_vao& operator=(const path_vao&) = delete;

	path_vao(path_vao&&) = default;
	path_vao& operator=(path_vao&&) = default;

	/**
	 * @brief Default destructor.
	 */
	~path_vao() = default;

	/**
	 * @brief Set path vertices to render.
	 * First build a path by adding points to it, then stroke the path with
	 * path::stroke() to obtain the vertices, and finally pass them to the
	 * vao using this method.
	 * @param path - path vertices as produced by path::stroke().
	 */
	void set(const path::vertices& path);

	/**
	 * @brief Render the path.
	 * @param matrix - model-view-projection matrix.
	 * @param color - color to render the path with.
	 */
	void render(
		const ruis::mat4& matrix, //
		ruis::color color
	) const;
};

} // namespace ruis::paint
