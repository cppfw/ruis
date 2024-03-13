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

#include <tml/tree.hpp>

#include "util/units.hpp"

#include "config.hpp"

namespace ruis {

/**
 * @brief Layout parameters.
 */
struct lp {
	/**
	 * @brief Alignment parameter values.
	 */
	enum class align {
		/**
		 * @brief Left or top alignment.
		 */
		front,

		/**
		 * @brief Center lignment.
		 */
		center,

		/**
		 * @brief Right or bottom alignment.
		 */
		back
	};

	/**
	 * @brief Requests minimal dimensions of the widget.
	 * The widget will always be given minimal space it needs to properly draw.
	 */
	constexpr static const real min = real(-1);

	/**
	 * @brief Requests minimal or bigger dimensions of widget.
	 * The widget will be given at least minimal space it needs to properly draw.
	 * 'max' behaves the same way as 'min' during measure, but during layouting
	 * the widget will be given same size as parent.
	 */
	constexpr static const real max = real(-2);

	/**
	 * @brief Requests widget to be same size as its parent.
	 * Minimal size of the widget is assumed to be 0.
	 */
	constexpr static const real fill = real(-3);

	/**
	 * @brief desired dimensions.
	 * Components should hold non-negative value in pixels or [min, max, fill].
	 */
	vector2 dims = vector2(lp::min);

	/**
	 * @brief Weight of the widget.
	 * Weight defines how much space widget occupies in addition to its minimal or explicitly set size.
	 * Default value is 0, which means that the widget will not occupy extra space.
	 */
	real weight = 0;

	/**
	 * @brief Alignment of the widget within its parent.
	 * Horizontal and vertical alignment.
	 */
	r4::vector2<align> align = {align::center, align::center};

	static lp make(const tml::forest& desc, const ruis::units& units);
};

}; // namespace ruis
