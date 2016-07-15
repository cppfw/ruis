#pragma once

#include <utki/config.hpp>


#if M_OS == M_OS_WINDOWS
#	ifdef DELETE
#		undef DELETE
#	endif
#endif


namespace morda{


/**
 * @brief Keyboard keys enumeration.
 */
enum class Key_e{
//WARNING: do not add new key codes in the middle and do not change order. Add new key codes to the end of the enumeration.
	UNKNOWN,
	SPACE,
	ENTER,
	ZERO,
	ONE,
	TWO, //5
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN, //10
	EIGHT,
	NINE,
	A,
	B,
	C, //15
	D,
	E,
	F,
	G,
	H, //20
	I,
	J,
	K,
	L,
	M, //25
	N,
	O,
	P,
	Q,
	R, //30
	S,
	T,
	U,
	V,
	W, //35
	X,
	Y,
	Z,
	LEFT,
	RIGHT, //40
	UP,
	DOWN,
	COMMA,
	SEMICOLON,
	APOSTROPHE,//', 45
	PERIOD,
	SLASH,
	BACKSLASH,
	TAB,
	LEFT_SHIFT, //50
	RIGHT_SHIFT,
	END,
	LEFT_SQUARE_BRACKET,
	RIGHT_SQUARE_BRACKET,
	GRAVE, //`, 55
	MINUS,
	EQUALS,
	BACKSPACE,
	CAPSLOCK,
	ESCAPE, //60
	LEFT_CONTROL,
	LEFT_ALT,
	F1,
	F2,
	F3, //65
	F4,
	F5,
	F6,
	F7,
	F8, //70
	F9,
	F10,
	F11,
	F12,
	RIGHT_CONTROL, //75
	PRINT_SCREEN,
	RIGHT_ALT,
	HOME,
	PAGE_UP,
	PAGE_DOWN, //80
	INSERT,
	DELETE,
	PAUSE,
	WINDOWS,
	WINDOWS_MENU, //85
	FUNCTION,
	F17,
	F18,
	F19,
	F20, //90
	F13,
	F16,
	F14,
	F15,
	
	ENUM_SIZE
};



}//~namespace
