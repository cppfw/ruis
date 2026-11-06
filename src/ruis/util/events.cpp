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

#include "events.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <utility>

using namespace std::string_view_literals;

namespace {
constexpr auto button_action_name_mapping = []() constexpr {
	auto arr = std::array<std::pair<std::string_view, ruis::button_action>, 2>{
		{{"press"sv, ruis::button_action::press}, {"release"sv, ruis::button_action::release}}
	};
	std::ranges::sort(arr);
	return arr;
}();

constexpr auto mouse_button_name_mapping = []() constexpr {
	auto arr = std::array<std::pair<std::string_view, ruis::mouse_button>, size_t(ruis::mouse_button::enum_size)>{
		{{"left"sv, ruis::mouse_button::left},
		 {"right"sv, ruis::mouse_button::right},
		 {"middle"sv, ruis::mouse_button::middle},
		 {"wheel_up"sv, ruis::mouse_button::wheel_up},
		 {"wheel_down"sv, ruis::mouse_button::wheel_down},
		 {"wheel_left"sv, ruis::mouse_button::wheel_left},
		 {"wheel_right"sv, ruis::mouse_button::wheel_right},
		 {"side"sv, ruis::mouse_button::side},
		 {"extra"sv, ruis::mouse_button::extra},
		 {"forward"sv, ruis::mouse_button::forward},
		 {"back"sv, ruis::mouse_button::back},
		 {"task"sv, ruis::mouse_button::task}}
	};
	std::ranges::sort(arr);
	return arr;
}();
} // namespace

ruis::button_action ruis::to_button_action(std::string_view name)
{
	auto i = std::ranges::lower_bound(
		button_action_name_mapping, //
		name,
		{}, // comparison function, empty means to use operator<()
		&std::pair<std::string_view, ruis::button_action>::first
	);
	if (i != button_action_name_mapping.end() && i->first == name) {
		return i->second;
	}
	return ruis::button_action::enum_size;
}

ruis::mouse_button ruis::to_mouse_button(std::string_view name)
{
	auto i = std::ranges::lower_bound(
		mouse_button_name_mapping, //
		name,
		{}, // comparison function, empty means to use operator<()
		&std::pair<std::string_view, ruis::mouse_button>::first
	);
	if (i != mouse_button_name_mapping.end() && i->first == name) {
		return i->second;
	}
	return ruis::mouse_button::enum_size;
}
