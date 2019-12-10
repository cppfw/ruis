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
enum class key{
//WARNING: do not add new key codes in the middle and do not change order. Add new key codes to the end of the enumeration.
	unknown,
	space,
	enter,
	zero,
	one,
	two, //5
	three,
	four,
	five,
	six,
	seven, //10
	eight,
	nine,
	a,
	b,
	c, //15
	d,
	e,
	f,
	g,
	h, //20
	i,
	j,
	k,
	l,
	m, //25
	n,
	o,
	p,
	q,
	r, //30
	s,
	t,
	u,
	v,
	w, //35
	x,
	y,
	z,
	left,
	right, //40
	up,
	down,
	comma,
	semicolon,
	apostrophe,//', 45
	period,
	slash,
	backslash,
	tabulator,
	left_shift, //50
	right_shift,
	end,
	left_square_bracket,
	right_square_bracket,
	grave, //`, 55
	minus,
	equals,
	backspace,
	capslock,
	escape, //60
	left_control,
	left_alt,
	f1,
	f2,
	f3, //65
	f4,
	f5,
	f6,
	f7,
	f8, //70
	f9,
	f10,
	f11,
	f12,
	right_control, //75
	print_screen,
	right_alt,
	home,
	page_up,
	page_down, //80
	insert,
	deletion,
	pause,
	command,
	menu, //85
	function,
	f17,
	f18,
	f19,
	f20, //90
	f13,
	f16,
	f14,
	f15,
	
	ENUM_SIZE
};

}
