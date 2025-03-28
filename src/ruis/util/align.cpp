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

#include "align.hpp"

using namespace std::string_view_literals;

using namespace ruis;

align ruis::make_from(const tml::forest& desc, const align& adl)
{
	if (desc.empty()) {
		return align::undefined;
	}

	const auto& s = desc.front().value.string;

	if (s == "front"sv) {
		return align::front;
	} else if (s == "center"sv) {
		return align::center;
	} else if (s == "back"sv) {
		return align::back;
	}
	return align::undefined;
}
