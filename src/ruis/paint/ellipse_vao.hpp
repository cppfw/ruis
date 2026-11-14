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

#include "../config.hpp"
#include "../render/renderer.hpp"

namespace ruis::paint {

/**
 * @brief A vertex array object which renders an ellipse.
 * An ellipse_vao can render either a filled or a stroked ellipse.
 */
class ellipse_vao
{
	utki::shared_ref<const ruis::render::renderer> renderer;

	vec2 diameters = 0;

	// non-positive value means filled ellipse
	real stroke_width = 0;

public:
	/**
	 * @brief Constructor.
	 * @param renderer - renderer to use.
	 * @param diameters - ellipse diameters.
	 * @param stroke_width - ellipse border width. Non-positive value means filled ellipse.
	 */
	ellipse_vao(
		utki::shared_ref<const ruis::render::renderer> renderer,
		vec2 diameters = 0, //
		real stroke_width = 0 //
	);

	ellipse_vao(const ellipse_vao&) = delete;
	ellipse_vao& operator=(const ellipse_vao&) = delete;

	ellipse_vao(ellipse_vao&&) = delete;
	ellipse_vao& operator=(ellipse_vao&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	~ellipse_vao() = default;

	/**
	 * @brief Set ellipse parameters.
	 * @param diameters - ellipse diameters.
	 * @param stroke_width - ellipse border width. Non-positive value means filled ellipse.
	 */
	void set(
		vec2 diameters, //
		real stroke_width = 0 // non-positive means filled ellipse
	);

	/**
	 * @brief Render the ellipse.
	 * @param matrix - model-view-projection matrix.
	 * @param color - color to render the ellipse with.
	 */
	void render(
		const ruis::mat4& matrix, //
		const ruis::color& color
	) const;

private:
	std::shared_ptr<const render::texture_2d> tex;

	/**
	 * @brief Key for the ellipse texture cache.
	 */
	struct texture_cache_key {
		/**
		 * @brief Ellipse diameters.
		 */
		vec2 diameters;

		/**
		 * @brief Ellipse border width.
		 */
		real stroke_width;

		/**
		 * @brief Less-than comparison operator.
		 * @param rhs - other cache key.
		 * @return true if this key is less than the other one.
		 */
		bool operator<(const texture_cache_key& rhs) const
		{
			if (this->diameters == rhs.diameters) {
				return this->stroke_width < rhs.stroke_width;
			}
			return this->diameters < rhs.diameters;
		}
	};

	static std::map<
		texture_cache_key, //
		std::weak_ptr<const render::texture_2d>>
		// TODO: ? NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
		cache;

	/**
	 * @brief Update the cached ellipse texture if the current parameters
	 * require a different texture than the one currently in use.
	 */
	void update_texture();
};
} // namespace ruis::paint
