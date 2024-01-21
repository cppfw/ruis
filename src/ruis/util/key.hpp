/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include <string_view>

#include <utki/flags.hpp>

namespace morda {

/**
 * @brief Keyboard keys enumeration.
 */
enum class key {
	space,
	enter,
	zero,
	one,
	two,
	three,
	four,
	five,
	six,
	seven,
	eight,
	nine,
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	j,
	k,
	l,
	m,
	n,
	o,
	p,
	q,
	r,
	s,
	t,
	u,
	v,
	w,
	x,
	y,
	z,
	arrow_left,
	arrow_right,
	arrow_up,
	arrow_down,
	comma,
	semicolon,
	apostrophe, // '
	period,
	slash,
	backslash,
	tabulator,
	left_shift,
	right_shift,
	end,
	left_square_bracket,
	right_square_bracket,
	grave, // `
	minus,
	equals,
	backspace,
	capslock,
	escape,
	left_control,
	left_alt,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12,
	right_control,
	print_screen,
	right_alt,
	home,
	page_up,
	page_down,
	insert,
	deletion,
	pause,
	left_command,
	right_command,
	menu,
	function,
	f17,
	f18,
	f19,
	f20,
	f13,
	f16,
	f14,
	f15,

	enum_size,
	unknown = enum_size
};

enum class key_modifier {
	left_shift,
	right_shift,
	left_alt,
	right_alt,
	left_control,
	right_control,
	left_command,
	right_command,

	enum_size,
	unknown = enum_size
};

key_modifier to_key_modifier(morda::key key);

std::string_view to_string(morda::key key);

morda::key to_key(std::string_view name);

struct key_combo {
	morda::key key = morda::key::unknown;
	utki::flags<key_modifier> modifiers;

	bool operator==(const key_combo& kc) const
	{
		return this->key == kc.key && this->modifiers == kc.modifiers;
	}

	bool operator!=(const key_combo& kc) const
	{
		return !this->operator==(kc);
	}
};

} // namespace morda
