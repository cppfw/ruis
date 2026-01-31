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

#include "../scroll_area.hpp"

namespace ruis::touch {

class scroll_area : public ruis::scroll_area
{
	constexpr static auto scroll_threshold_pp = 5;

	// TODO: convert from scroll_threshold_pp in on_reload()?
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "constant value")
	real scroll_threshold_px = 10;

	enum class state {
		idle,
		within_scroll_threshold,
		not_scrolling,
		scrolling
	} cur_state = state::idle;

	vec2 prev_touch_point;

	// Scroll area only works in single touch mode.
	// This variable holds the pointer ID of the current touch.
	unsigned cur_pointer_id = std::numeric_limits<unsigned>::max();

public:
	scroll_area(
		utki::shared_ref<ruis::context> context, //
		scroll_area::all_parameters params,
		widget_list children
	);

	bool on_mouse_button(const mouse_button_event& event) override;

	bool on_mouse_move(const mouse_move_event& event) override;
};

} // namespace ruis::touch
