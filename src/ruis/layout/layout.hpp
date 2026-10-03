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
#include "../util/widget_list.hpp"

#include "measure_mode.hpp"

namespace ruis {

// TODO: doxygen
class layout
{
protected:
	layout() = default;

public:
	layout(const layout&) = delete;
	layout& operator=(const layout&) = delete;

	layout(layout&&) = delete;
	layout& operator=(layout&&) = delete;

	/**
	 * @brief Measure how big the layout wants to be.
	 * @param quotum - space available to the layout.
	 * @param mode - measurement mode for each dimension. In at_most mode the widget is
	 * expected to report the minimal size it needs (clamped to the quotum), in exactly
	 * mode the widget is expected to be resized to the given exact size.
	 * @param widgets - widgets to measure.
	 * @return Measured desired layout dimensions.
	 */
	virtual vec2 measure(
		const vec2& quotum, //
		const r4::vector2<measure_mode>& mode, //
		const_widget_list& widgets
	) const = 0;

	/**
	 * @brief Arrange widgets.
	 * @param container_dims - dimensions of the area available to the layout.
	 * @param widgets - widgets to arrange.
	 */
	virtual void lay_out(
		const vec2& container_dims, //
		semiconst_widget_list& widgets
	) const = 0;

	virtual ~layout() = default;

	static const utki::shared_ref<layout> trivial;
	static const utki::shared_ref<layout> size;
	static const utki::shared_ref<layout> pile;
	static const utki::shared_ref<layout> row;
	static const utki::shared_ref<layout> column;
};

} // namespace ruis
