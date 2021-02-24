#pragma once

#include "../config.hpp"

namespace morda{

/**
 * @brief Mouse buttons enumeration.
 */
enum class mouse_button{
	left,
	right,
	middle,
	wheel_up,
	wheel_down,

	enum_size
};

struct mouse_button_event{
	bool is_down; /// was the button pressed (true) or released (false)
	vector2 pos; /// position of the mouse cursor at the moment when the button event has occurred, in widget local coordinates
	mouse_button button; /// mouse button
	unsigned pointer_id; /// id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens
};

struct mouse_move_event{
	vector2 pos; /// position of the mouse cursor at the moment when the button event has occurred, in widget local coordinates
	unsigned pointer_id; /// id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens
	bool ignore_mouse_capture; /// ignore mouse capturing and distribute mouse move event to all child widgets
};

}
