#pragma once


namespace morda{

/**
 * @brief Mouse buttons enumeration.
 */
enum class mouse_button{
	left,
	LEFT = left, //TODO: deprecated, remove.
	right,
	RIGHT = right, //TODO: deprecated, remove.
	middle,
	MIDDLE = middle, //TODO: deprecated, remove.
	wheel_up,
	WHEEL_UP = wheel_up, //TODO: deprecated, remove.
	wheel_down,
	WHEEL_DOWN = wheel_down, //TODO: deprecated, remove.

	ENUM_SIZE
};

//TODO: deprecated, remove.
typedef mouse_button MouseButton_e;

}