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

#include <deque>

#include "../../widget.hpp"

namespace ruis::touch {

class flickable :
	virtual public ruis::widget, //
	private updateable
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

	// So far, the flickable only works in single touch mode.
	// This variable holds the pointer ID of the current touch.
	unsigned cur_pointer_id = std::numeric_limits<unsigned>::max();

	struct touch_move_info{
		vec2 position;
		uint32_t timestamp_ms;
	};

	std::deque<touch_move_info> touch_history;

	void push_touch_move_to_history(touch_move_info tm);
	vec2 calculate_touch_velocity();

public:
	event_status on_mouse_button(const mouse_button_event& event) override;
	event_status on_mouse_move(const mouse_move_event& event) override;

protected:
	// TODO: doxygen
	virtual event_status flickable_on_mouse_button(const mouse_button_event& event) = 0;
	virtual event_status flickable_on_mouse_move(const mouse_move_event& event) = 0;

	/**
	 * @brief Scroll by given number of pixels.
	 * @param delta - number of pixels to scroll by.
	 * @return Number of pixels actually scrolled.
	 */
	virtual vec2 flickable_scroll_by(const vec2& delta) = 0;

private:
	void update(uint32_t dt_ms) override;
};

} // namespace ruis::touch
