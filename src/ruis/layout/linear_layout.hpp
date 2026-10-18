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

#include "../util/oriented.hpp"

#include "layout.hpp"

namespace ruis {

/**
 * @brief Layout that arranges the widgets in a single line, horizontally or vertically.
 *
 * The direction of the line is called the longitudinal direction, the other direction is
 * called the transverse direction.
 *
 * A widget is always resized to the space assigned to it in the longitudinal direction,
 * therefore the alignment in the longitudinal direction has no effect, only the alignment
 * in the transverse direction is used.
 *
 * The space assigned to a widget in the longitudinal direction is:
 * - its measured size for 'min', 'length' and 'undefined' dimensions, zero for 'fill' and
 *   'max' dimensions;
 * - plus its share of the space left over after the measured sizes of the widgets, if the
 *   widget has a weight. The shares are proportional to the weights.
 * The assigned space is clamped to the space left in the container, so a widget never
 * overflows the container.
 *
 * In the transverse direction a 'fill' or 'max' widget is resized to the size of the
 * container, a 'min' or 'undefined' widget is given its measured size clamped to the size
 * of the container and a 'length' widget is given the specified length.
 *
 * The measured size of the container is the sum of the measured longitudinal sizes of the
 * 'min', 'length' and 'undefined' widgets, the 'fill' and 'max' widgets contribute zero to
 * that sum.
 */
class linear_layout :
	public layout, //
	public oriented
{
public:
	linear_layout(bool is_vertical);

	void lay_out(
		const vec2& dims, //
		semiconst_widget_list& widgets
	) const override;

	vec2 measure(
		const vec2& quotum, //
		const r4::vector2<measure_mode>& mode, //
		const_widget_list& widgets
	) const override;
};

} // namespace ruis
