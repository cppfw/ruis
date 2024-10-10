/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "format.hpp"

#include <sstream>

using namespace ruis;

namespace {
std::tuple<unsigned, std::u32string_view::const_iterator> read_number(
	std::u32string_view::const_iterator begin, //
	std::u32string_view::const_iterator end
)
{
	// TODO: read until }
	return std::make_tuple(0, begin);
}
} // namespace

std::u32string ruis::format(
	std::u32string_view fmt, //
	utki::span<const std::u32string> args
)
{
	std::basic_stringstream<char32_t> ss;

	for (auto pos = fmt.cbegin(); pos != fmt.cend();) {
		auto c = *pos;
		if (c == U'{') {
			auto [number, new_pos] = read_number(pos, fmt.cend());
			pos = new_pos;
			ss << args.at(number);
		} else {
			ss << c;
			++pos;
		}
	}

	return ss.str();
}
