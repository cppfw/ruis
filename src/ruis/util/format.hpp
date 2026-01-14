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

#pragma once

#include <utki/span.hpp>

namespace ruis {

struct format_chunk {
	std::u32string_view chunk;
	unsigned replacement_id;
};

std::vector<format_chunk> parse_format(std::u32string_view fmt);

std::u32string format(
	utki::span<const format_chunk> fmt, //
	utki::span<const std::u32string> args
);

inline std::u32string format(
	std::u32string_view fmt, //
	utki::span<const std::u32string> args
)
{
	return format(parse_format(fmt), args);
}

} // namespace ruis
