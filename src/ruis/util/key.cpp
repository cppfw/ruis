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

#include "key.hpp"

#include <algorithm>
#include <array>

#include <utki/enum_array.hpp>
#include <utki/sort.hpp>

using namespace ruis;

using namespace std::string_view_literals;

key_modifier ruis::to_key_modifier(ruis::key key)
{
	switch (key) {
		case key::left_shift:
			return key_modifier::left_shift;
		case key::right_shift:
			return key_modifier::right_shift;
		case key::left_alt:
			return key_modifier::left_alt;
		case key::right_alt:
			return key_modifier::right_alt;
		case key::left_control:
			return key_modifier::left_control;
		case key::right_control:
			return key_modifier::right_control;
		case key::left_command:
			return key_modifier::left_command;
		case key::right_command:
			return key_modifier::right_command;
		default:
			return key_modifier::unknown;
	}
}

namespace {
// the order of strings must correspond to the order of ruis::key enum items
constexpr utki::enum_array<std::string_view, ruis::key> key_names = {
	"space"sv,
	"enter"sv,
	"0"sv,
	"1"sv,
	"2"sv,
	"3"sv,
	"4"sv,
	"5"sv,
	"6"sv,
	"7"sv,
	"8"sv,
	"9"sv,
	"a"sv,
	"b"sv,
	"c"sv,
	"d"sv,
	"e"sv,
	"f"sv,
	"g"sv,
	"h"sv,
	"i"sv,
	"j"sv,
	"k"sv,
	"l"sv,
	"m"sv,
	"n"sv,
	"o"sv,
	"p"sv,
	"q"sv,
	"r"sv,
	"s"sv,
	"t"sv,
	"u"sv,
	"v"sv,
	"w"sv,
	"x"sv,
	"y"sv,
	"z"sv,
	"arrow_left"sv,
	"arrow_right"sv,
	"arrow_up"sv,
	"arrow_down"sv,
	"comma"sv,
	"semicolon"sv,
	"apostrophe"sv,
	"period"sv,
	"slash"sv,
	"backslash"sv,
	"tabulator"sv,
	"left_shift"sv,
	"right_shift"sv,
	"end"sv,
	"left_square_bracket"sv,
	"right_square_bracket"sv,
	"grave"sv,
	"minus"sv,
	"equals"sv,
	"backspace"sv,
	"capslock"sv,
	"escape"sv,
	"left_control"sv,
	"left_alt"sv,
	"f1"sv,
	"f2"sv,
	"f3"sv,
	"f4"sv,
	"f5"sv,
	"f6"sv,
	"f7"sv,
	"f8"sv,
	"f9"sv,
	"f10"sv,
	"f11"sv,
	"f12"sv,
	"right_control"sv,
	"print_screen"sv,
	"right_alt"sv,
	"home"sv,
	"page_up"sv,
	"page_down"sv,
	"insert"sv,
	"deletion"sv,
	"pause"sv,
	"left_command"sv,
	"right_command"sv,
	"menu"sv,
	"function"sv,
	"f17"sv,
	"f18"sv,
	"f19"sv,
	"f20"sv,
	"f13"sv,
	"f16"sv,
	"f14"sv,
	"f15"sv
};
} // namespace

std::string_view ruis::to_string(ruis::key key)
{
	if (key >= ruis::key::enum_size) {
		return "unknown"sv;
	}

	return key_names[key];
}

namespace {
struct key_name_key_pair {
	std::string_view first;
	ruis::key second;
};

constexpr auto key_name_to_key_ordered_mapping = []() constexpr {
	utki::enum_array<key_name_key_pair, ruis::key> arr = {{{
		{"space"sv, ruis::key::space},
		{"enter"sv, ruis::key::enter},
		{"0"sv, ruis::key::zero},
		{"1"sv, ruis::key::one},
		{"2"sv, ruis::key::two},
		{"3"sv, ruis::key::three},
		{"4"sv, ruis::key::four},
		{"5"sv, ruis::key::five},
		{"6"sv, ruis::key::six},
		{"7"sv, ruis::key::seven},
		{"8"sv, ruis::key::eight},
		{"9"sv, ruis::key::nine},
		{"a"sv, ruis::key::a},
		{"b"sv, ruis::key::b},
		{"c"sv, ruis::key::c},
		{"d"sv, ruis::key::d},
		{"e"sv, ruis::key::e},
		{"f"sv, ruis::key::f},
		{"g"sv, ruis::key::g},
		{"h"sv, ruis::key::h},
		{"i"sv, ruis::key::i},
		{"j"sv, ruis::key::j},
		{"k"sv, ruis::key::k},
		{"l"sv, ruis::key::l},
		{"m"sv, ruis::key::m},
		{"n"sv, ruis::key::n},
		{"o"sv, ruis::key::o},
		{"p"sv, ruis::key::p},
		{"q"sv, ruis::key::q},
		{"r"sv, ruis::key::r},
		{"s"sv, ruis::key::s},
		{"t"sv, ruis::key::t},
		{"u"sv, ruis::key::u},
		{"v"sv, ruis::key::v},
		{"w"sv, ruis::key::w},
		{"x"sv, ruis::key::x},
		{"y"sv, ruis::key::y},
		{"z"sv, ruis::key::z},
		{"arrow_left"sv, ruis::key::arrow_left},
		{"arrow_right"sv, ruis::key::arrow_right},
		{"arrow_up"sv, ruis::key::arrow_up},
		{"arrow_down"sv, ruis::key::arrow_down},
		{"comma"sv, ruis::key::comma},
		{"semicolon"sv, ruis::key::semicolon},
		{"apostrophe"sv, ruis::key::apostrophe},
		{"period"sv, ruis::key::period},
		{"slash"sv, ruis::key::slash},
		{"backslash"sv, ruis::key::backslash},
		{"tabulator"sv, ruis::key::tabulator},
		{"left_shift"sv, ruis::key::left_shift},
		{"right_shift"sv, ruis::key::right_shift},
		{"end"sv, ruis::key::end},
		{"left_square_bracket"sv, ruis::key::left_square_bracket},
		{"right_square_bracket"sv, ruis::key::right_square_bracket},
		{"grave"sv, ruis::key::grave},
		{"minus"sv, ruis::key::minus},
		{"equals"sv, ruis::key::equals},
		{"backspace"sv, ruis::key::backspace},
		{"capslock"sv, ruis::key::capslock},
		{"escape"sv, ruis::key::escape},
		{"left_control"sv, ruis::key::left_control},
		{"left_alt"sv, ruis::key::left_alt},
		{"f1"sv, ruis::key::f1},
		{"f2"sv, ruis::key::f2},
		{"f3"sv, ruis::key::f3},
		{"f4"sv, ruis::key::f4},
		{"f5"sv, ruis::key::f5},
		{"f6"sv, ruis::key::f6},
		{"f7"sv, ruis::key::f7},
		{"f8"sv, ruis::key::f8},
		{"f9"sv, ruis::key::f9},
		{"f10"sv, ruis::key::f10},
		{"f11"sv, ruis::key::f11},
		{"f12"sv, ruis::key::f12},
		{"right_control"sv, ruis::key::right_control},
		{"print_screen"sv, ruis::key::print_screen},
		{"right_alt"sv, ruis::key::right_alt},
		{"home"sv, ruis::key::home},
		{"page_up"sv, ruis::key::page_up},
		{"page_down"sv, ruis::key::page_down},
		{"insert"sv, ruis::key::insert},
		{"deletion"sv, ruis::key::deletion},
		{"pause"sv, ruis::key::pause},
		{"left_command"sv, ruis::key::left_command},
		{"right_command"sv, ruis::key::right_command},
		{"menu"sv, ruis::key::menu},
		{"function"sv, ruis::key::function},
		{"f17"sv, ruis::key::f17},
		{"f18"sv, ruis::key::f18},
		{"f19"sv, ruis::key::f19},
		{"f20"sv, ruis::key::f20},
		{"f13"sv, ruis::key::f13},
		{"f16"sv, ruis::key::f16},
		{"f14"sv, ruis::key::f14},
		{"f15"sv, ruis::key::f15},
	}}};

	utki::sort(arr.begin(), arr.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
	});
	return arr;
}();
} // namespace

ruis::key ruis::to_key(std::string_view name)
{
	auto i = std::lower_bound(
		key_name_to_key_ordered_mapping.begin(),
		key_name_to_key_ordered_mapping.end(),
		name,
		[](const auto& a, const std::string_view& b) {
			return a.first < b;
		}
	);
	if (i != key_name_to_key_ordered_mapping.end() && i->first == name) {
		return i->second;
	}
	return ruis::key::unknown;
}
