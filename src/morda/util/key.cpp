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

#include "key.hpp"

#include <algorithm>
#include <array>

#include <utki/sort.hpp>

using namespace morda;

using namespace std::string_view_literals;

key_modifier morda::to_key_modifier(morda::key key)
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
// the order of strings must correspond to the order of morda::key enum items
constexpr std::array<std::string_view, size_t(morda::key::enum_size)> key_names = {
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
	"f15"sv};
} // namespace

std::string_view morda::to_string(morda::key key)
{
	if (key >= morda::key::enum_size) {
		return "unknown"sv;
	}
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	return key_names[size_t(key)];
}

namespace {
struct key_name_key_pair {
	std::string_view first;
	morda::key second;
};

constexpr auto key_name_to_key_ordered_mapping = []() constexpr {
	std::array<key_name_key_pair, size_t(morda::key::enum_size)> arr = {
		{
         {"space"sv, morda::key::space},
         {"enter"sv, morda::key::enter},
         {"0"sv, morda::key::zero},
         {"1"sv, morda::key::one},
         {"2"sv, morda::key::two},
         {"3"sv, morda::key::three},
         {"4"sv, morda::key::four},
         {"5"sv, morda::key::five},
         {"6"sv, morda::key::six},
         {"7"sv, morda::key::seven},
         {"8"sv, morda::key::eight},
         {"9"sv, morda::key::nine},
         {"a"sv, morda::key::a},
         {"b"sv, morda::key::b},
         {"c"sv, morda::key::c},
         {"d"sv, morda::key::d},
         {"e"sv, morda::key::e},
         {"f"sv, morda::key::f},
         {"g"sv, morda::key::g},
         {"h"sv, morda::key::h},
         {"i"sv, morda::key::i},
         {"j"sv, morda::key::j},
         {"k"sv, morda::key::k},
         {"l"sv, morda::key::l},
         {"m"sv, morda::key::m},
         {"n"sv, morda::key::n},
         {"o"sv, morda::key::o},
         {"p"sv, morda::key::p},
         {"q"sv, morda::key::q},
         {"r"sv, morda::key::r},
         {"s"sv, morda::key::s},
         {"t"sv, morda::key::t},
         {"u"sv, morda::key::u},
         {"v"sv, morda::key::v},
         {"w"sv, morda::key::w},
         {"x"sv, morda::key::x},
         {"y"sv, morda::key::y},
         {"z"sv, morda::key::z},
         {"arrow_left"sv, morda::key::arrow_left},
         {"arrow_right"sv, morda::key::arrow_right},
         {"arrow_up"sv, morda::key::arrow_up},
         {"arrow_down"sv, morda::key::arrow_down},
         {"comma"sv, morda::key::comma},
         {"semicolon"sv, morda::key::semicolon},
         {"apostrophe"sv, morda::key::apostrophe},
         {"period"sv, morda::key::period},
         {"slash"sv, morda::key::slash},
         {"backslash"sv, morda::key::backslash},
         {"tabulator"sv, morda::key::tabulator},
         {"left_shift"sv, morda::key::left_shift},
         {"right_shift"sv, morda::key::right_shift},
         {"end"sv, morda::key::end},
         {"left_square_bracket"sv, morda::key::left_square_bracket},
         {"right_square_bracket"sv, morda::key::right_square_bracket},
         {"grave"sv, morda::key::grave},
         {"minus"sv, morda::key::minus},
         {"equals"sv, morda::key::equals},
         {"backspace"sv, morda::key::backspace},
         {"capslock"sv, morda::key::capslock},
         {"escape"sv, morda::key::escape},
         {"left_control"sv, morda::key::left_control},
         {"left_alt"sv, morda::key::left_alt},
         {"f1"sv, morda::key::f1},
         {"f2"sv, morda::key::f2},
         {"f3"sv, morda::key::f3},
         {"f4"sv, morda::key::f4},
         {"f5"sv, morda::key::f5},
         {"f6"sv, morda::key::f6},
         {"f7"sv, morda::key::f7},
         {"f8"sv, morda::key::f8},
         {"f9"sv, morda::key::f9},
         {"f10"sv, morda::key::f10},
         {"f11"sv, morda::key::f11},
         {"f12"sv, morda::key::f12},
         {"right_control"sv, morda::key::right_control},
         {"print_screen"sv, morda::key::print_screen},
         {"right_alt"sv, morda::key::right_alt},
         {"home"sv, morda::key::home},
         {"page_up"sv, morda::key::page_up},
         {"page_down"sv, morda::key::page_down},
         {"insert"sv, morda::key::insert},
         {"deletion"sv, morda::key::deletion},
         {"pause"sv, morda::key::pause},
         {"left_command"sv, morda::key::left_command},
         {"right_command"sv, morda::key::right_command},
         {"menu"sv, morda::key::menu},
         {"function"sv, morda::key::function},
         {"f17"sv, morda::key::f17},
         {"f18"sv, morda::key::f18},
         {"f19"sv, morda::key::f19},
         {"f20"sv, morda::key::f20},
         {"f13"sv, morda::key::f13},
         {"f16"sv, morda::key::f16},
         {"f14"sv, morda::key::f14},
         {"f15"sv, morda::key::f15},
		 }
    };

	utki::sort(arr.begin(), arr.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
	});
	return arr;
}();
} // namespace

morda::key morda::to_key(std::string_view name)
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
	return morda::key::unknown;
}
