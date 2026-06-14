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

#include "../container.hpp"

namespace ruis {

/**
 * @brief Drag area container.
 * The area for free floating widgets which can be dragged and resized by mouse, for example windows.
 * This is very similar to an ordinary container with trivial layout.
 * The difference is that this container scales and moves its children when dots per pp value changes,
 * for example due to window moving to a different display with a different DPI.
 */
class drag_area : public container
{
	real cur_dots_per_pp;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
	};

	drag_area(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	void on_reload() override;
};

namespace make {
utki::shared_ref<ruis::drag_area> drag_area(
	utki::shared_ref<ruis::context> context, //
	ruis::drag_area::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis
