/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include <utki/flags.hpp>

#include "../config.hpp"

#include "key.hpp"

namespace ruis {

/**
 * @brief Mouse buttons enumeration.
 */
enum class mouse_button {
	left,
	right,
	middle,
	wheel_up,
	wheel_down,
	wheel_left,
	wheel_right,
	side,
	extra,
	forward,
	back,
	task,

	enum_size
};

struct mouse_button_event {
	bool is_down; /// was the button pressed (true) or released (false)
	vector2 pos; /// position of the mouse cursor at the moment when the button event has occurred, in widget local
				 /// coordinates
	mouse_button button; /// mouse button
	unsigned pointer_id; /// id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens
};

struct mouse_move_event {
	vector2 pos; /// position of the mouse cursor at the moment when the button event has occurred, in widget local
				 /// coordinates
	unsigned pointer_id; /// id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens
	bool ignore_mouse_capture; /// ignore mouse capturing and distribute mouse move event to all child widgets
};

struct key_event {
	bool is_down = true;
	key_combo combo;
};

struct character_input_event {
	std::u32string_view string;

	/**
	 * @brief Key code associated with the character input.
	 * Can be unknown.
	 */
	key_combo combo;
};

} // namespace ruis
