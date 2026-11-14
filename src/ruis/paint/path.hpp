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

#include <vector>

#include "../config.hpp"

namespace ruis::paint {

/**
 * @brief A 2D vector path.
 * A path is an ordered sequence of points which can be stroked into a set of
 * vertices for rendering.
 */
class path
{
	std::vector<ruis::vec2> points = {{ruis::vec2(0)}};

public:
	/**
	 * @brief Default constructor.
	 * Creates an empty path.
	 */
	path() = default;

	path(const path&) = delete;
	path& operator=(const path&) = delete;

	path(path&&) = delete;
	path& operator=(path&&) = delete;

	~path() = default;

	// TODO: rename to line_abs/line_rel etc. similar to how it is done in veg
	/**
	 * @brief Add an absolute line segment to the path.
	 * @param abs_pos - absolute end point of the line segment.
	 */
	void line_to(ruis::vec2 abs_pos);

	/**
	 * @brief Add an absolute line segment to the path.
	 * @param x - x coordinate of the absolute end point of the line segment.
	 * @param y - y coordinate of the absolute end point of the line segment.
	 */
	void line_to(
		ruis::real x, //
		ruis::real y
	)
	{
		this->line_to(ruis::vec2(x, y));
	}

	/**
	 * @brief Add a relative line segment to the path.
	 * @param rel_pos - end point of the line segment relative to the current point.
	 */
	void line_by(ruis::vec2 rel_pos);

	/**
	 * @brief Add an absolute cubic bezier curve to the path.
	 * @param abs_p1 - absolute first control point.
	 * @param abs_p2 - absolute second control point.
	 * @param abs_p3 - absolute end point of the curve.
	 */
	void cubic_to(
		ruis::vec2 abs_p1, //
		ruis::vec2 abs_p2, //
		ruis::vec2 abs_p3
	);

	/**
	 * @brief Add a relative cubic bezier curve to the path.
	 * @param rel_p1 - first control point relative to the current point.
	 * @param rel_p2 - second control point relative to the current point.
	 * @param rel_p3 - end point of the curve relative to the current point.
	 */
	void cubic_by(
		ruis::vec2 rel_p1, //
		ruis::vec2 rel_p2, //
		ruis::vec2 rel_p3
	);

	/**
	 * @brief Vertices produced by stroking a path.
	 */
	struct vertices {
		/**
		 * @brief Vertex positions.
		 */
		std::vector<ruis::vec2> pos;

		/**
		 * @brief Per-vertex alpha values used for anti-aliasing.
		 */
		std::vector<ruis::real> alpha;

		/**
		 * @brief Indices of the inner edge of the stroke.
		 */
		std::vector<uint16_t> in_indices;

		/**
		 * @brief Indices of the outer edge of the stroke.
		 */
		std::vector<uint16_t> out_indices;
	};

	/**
	 * @brief Default half width of the stroke.
	 */
	constexpr static auto default_half_width = 0.5;

	/**
	 * @brief Default width of the anti-aliased edge of the stroke.
	 */
	constexpr static auto default_antialias_width = 1;

	/**
	 * @brief Default alpha of the anti-aliased edge of the stroke.
	 */
	constexpr static auto default_antialias_alpha = 0.35;

	/**
	 * @brief Stroke the path, producing a set of vertices for rendering.
	 * @param half_width - half width of the stroke.
	 * @param antialias_width - width of the anti-aliased edge of the stroke.
	 * @param antialias_alpha - alpha of the anti-aliased edge of the stroke.
	 * @return vertices for rendering the stroked path.
	 */
	vertices stroke(
		ruis::real half_width = ruis::real(default_half_width),
		ruis::real antialias_width = ruis::real(default_antialias_width),
		ruis::real antialias_alpha = ruis::real(default_antialias_alpha)
	) const;
};

} // namespace ruis::paint
