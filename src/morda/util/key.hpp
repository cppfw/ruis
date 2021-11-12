/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include <utki/config.hpp>

namespace morda{

/**
 * @brief Keyboard keys enumeration.
 */
enum class key{
	// WARNING: do not add new key codes in the middle and do not change order. Add new key codes to the end of the enumeration.
	//          This is in order to preserve binary compatibility with mordavokne.
	unknown, // 0
	space,
	enter,
	zero,
	one,
	two, // 5
	three,
	four,
	five,
	six,
	seven, // 10
	eight,
	nine,
	a,
	b,
	c, // 15
	d,
	e,
	f,
	g,
	h, // 20
	i,
	j,
	k,
	l,
	m, // 25
	n,
	o,
	p,
	q,
	r, // 30
	s,
	t,
	u,
	v,
	w, // 35
	x,
	y,
	z,
	left,
	right, // 40
	up,
	down,
	comma,
	semicolon,
	apostrophe,//', 45
	period,
	slash,
	backslash,
	tabulator,
	left_shift, // 50
	right_shift,
	end,
	left_square_bracket,
	right_square_bracket,
	grave, //`, 55
	minus,
	equals,
	backspace,
	capslock,
	escape, // 60
	left_control,
	left_alt,
	f1,
	f2,
	f3, // 65
	f4,
	f5,
	f6,
	f7,
	f8, // 70
	f9,
	f10,
	f11,
	f12,
	right_control, // 75
	print_screen,
	right_alt,
	home,
	page_up,
	page_down, // 80
	insert,
	deletion,
	pause,
	left_command,
	right_command, // 85
	menu,
	function,
	f17,
	f18,
	f19, // 90
	f20,
	f13,
	f16,
	f14,
	f15, // 95

	// WARNING: do not add new key codes in the middle and do not change order. Add new key codes to the end of the enumeration.
	//          This is in order to preserve binary compatibility with mordavokne.
	
	enum_size
};

enum class key_modifiers{
	left_shift,
	right_shift,
	left_alt,
	right_alt,
	left_control,
	right_control,
	left_command,
	right_command
};

}
