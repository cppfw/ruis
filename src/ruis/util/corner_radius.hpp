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

#include <algorithm>
#include <cmath>
#include <tuple>
#include <utility>

#include "../config.hpp"

#include "corners.hpp"

namespace ruis {

/**
 * @brief Scale down corner radii so that the resulting rounded rectangle is valid.
 *
 * A rounded rectangle is a simple (non-self-intersecting) shape if and only if,
 * on every edge, the two corner radii lying on that edge sum to at most the length
 * of that edge - the straight segment between the two corner arcs must be >= 0:
 *
 *     left_top + right_top     <= width      (top edge)
 *     left_bottom + right_bottom <= width     (bottom edge)
 *     left_top + left_bottom   <= height     (left edge)
 *     right_top + right_bottom <= height     (right edge)
 *
 * (These four constraints also imply that each individual radius is <= width and
 * <= height, so each quarter-arc fits inside its corner.)
 *
 * When a constraint is violated, the two radii on that edge are multiplied by the
 * same factor so that they fit the edge, preserving their relative ratio, and are
 * then rounded down to whole pixels. Rounding down guarantees that the two radii of
 * an edge never sum to more than the edge length (each rounded radius is at most a
 * pixel smaller than its exact value). This mirrors the CSS border-radius overflow
 * rule. Radii that already fit are left untouched, so the function is a no-op for
 * non-conflicting input.
 *
 * In particular, setting the two radii of one side to large equal values makes
 * them share that side equally, producing a capsule-like end.
 *
 * The edges are processed width-first (top, bottom) and then height (left, right).
 * Because every step only ever shrinks radii, an edge that has already been made to
 * fit can never be re-violated by a later step, so a single pass is sufficient and
 * the result is deterministic.
 *
 * @param radii - in/out corner radii to normalize.
 * @param w - width the radii must fit within (clamped to >= 0).
 * @param h - height the radii must fit within (clamped to >= 0).
 */
inline void normalize_corner_radii(corners<real>& radii, real w, real h)
{
	using std::floor;

	w = std::max(w, real(0));
	h = std::max(h, real(0));

	// shrink the two radii of one edge proportionally until they fit the edge
	// length, then round them down to whole pixels; the result is returned by value
	const auto fit_edge = [](real a, real b, real limit) -> std::pair<real, real> {
		const auto sum = a + b;
		if (sum > limit && sum > 0) {
			const auto k = limit / sum;
			// floor so the rounded pair never sums to more than the edge length
			return {floor(a * k), floor(b * k)};
		}
		return {a, b};
	};

	// top
	std::tie(
		radii.left_top(), //
		radii.right_top() //
	) =
		fit_edge(
			radii.left_top(), //
			radii.right_top(), //
			w //
		);
	// bottom
	std::tie(
		radii.left_bottom(), //
		radii.right_bottom() //
	) =
		fit_edge(
			radii.left_bottom(), //
			radii.right_bottom(), //
			w //
		);
	// left
	std::tie(
		radii.left_top(), //
		radii.left_bottom() //
	) =
		fit_edge(
			radii.left_top(), //
			radii.left_bottom(), //
			h //
		);
	// right
	std::tie(
		radii.right_top(), //
		radii.right_bottom() //
	) =
		fit_edge(
			radii.right_top(), //
			radii.right_bottom(), //
			h //
		);
}

} // namespace ruis
