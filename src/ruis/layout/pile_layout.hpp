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

#include "layout.hpp"

namespace ruis {

/**
 * @brief A pile layout.
 * A layout which stacks (piles) its child widgets on top of each other within
 * the same area.
 */
class pile_layout : public layout
{
public:
	/**
	 * @brief Arrange widgets.
	 * @param dims - dimensions of the area available to the layout.
	 * @param widgets - widgets to arrange.
	 */
	void lay_out(
		const vec2& dims, //
		semiconst_widget_list& widgets
	) const override;

	/**
	 * @brief Measure how big the layout wants to be.
	 * @param quotum - space available to the layout.
	 * @param mode - measurement mode for each dimension.
	 * @param widgets - widgets to measure.
	 * @return Measured desired layout dimensions.
	 */
	vec2 measure(
		const vec2& quotum, //
		const r4::vector2<measure_mode>& mode, //
		const_widget_list& widgets
	) const override;
};

} // namespace ruis
