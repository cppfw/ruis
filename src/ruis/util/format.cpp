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

#include "format.hpp"

#include <charconv>
#include <sstream>

#include <utki/string.hpp>

using namespace ruis;

namespace {
std::tuple<unsigned, std::u32string_view::const_iterator> read_number(
	std::u32string_view::const_iterator begin, //
	std::u32string_view::const_iterator end
)
{
	// TODO: use std::inplace_vector when switch to C++26
	std::array<char, 4> number_chars{};

	auto number_i = number_chars.begin();

	auto i = begin;
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic, "TODO: resolve when std::inplace_vector is available")
	for (; i != end; ++i, ++number_i) {
		if (*i == U'}') {
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic, "TODO: resolve when std::inplace_vector is available")
			++i;
			break;
		}

		if (number_i == number_chars.end()) {
			throw std::invalid_argument("format replacement id is too big");
		}

		*number_i = char(*i);
	}

	auto number_view = std::string_view(
		number_chars.data(), //
		std::distance(number_chars.begin(), number_i)
	);

	// TODO: use some higher level utility function to parse a number from a string_view than std::from_chars
	unsigned value{};
	auto res = std::from_chars(
		number_view.data(), //
		utki::end_pointer(number_view),
		value
	);

	if (res.ec != std::errc()) {
		throw std::invalid_argument(utki::cat(
			"could not parse format replacement field id: ", //
			number_view
		));
	}

	return std::make_tuple(value, i);
}
} // namespace

std::vector<format_chunk> ruis::parse_format(std::u32string_view fmt)
{
	std::vector<format_chunk> ret;

	// clang-format off
	format_chunk cur_chunk{
		// NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage, "false positive")
		.chunk = std::u32string_view(fmt.data(), 0), //
		.replacement_id = std::numeric_limits<unsigned>::max()
	};
	// clang-format on

	for (auto pos = fmt.cbegin(); pos != fmt.cend();) {
		auto c = *pos;
		pos = std::next(pos);
		if (c == U'{') {
			auto [number, new_pos] = read_number(pos, fmt.cend());
			pos = new_pos;
			cur_chunk.replacement_id = number;
			ret.push_back(std::move(cur_chunk));

			// clang-format off
			cur_chunk = {
				// NOLINTNEXTLINE(bugprone-string-constructor, "false positive")
				.chunk = std::u32string_view(&*pos, 0), //
				.replacement_id = std::numeric_limits<unsigned>::max()
			};
			// clang-format off
		} else {
			cur_chunk.chunk = std::u32string_view(cur_chunk.chunk.data(), cur_chunk.chunk.size() + 1);
		}
	}

	if (!cur_chunk.chunk.empty()) {
		ret.push_back(std::move(cur_chunk));
	}

	return ret;
}

std::u32string ruis::format(
	utki::span<const format_chunk> fmt, //
	utki::span<const std::u32string> args
)
{
	std::u32string ret;

	size_t capacity = 0;
	for (const auto& c : fmt) {
		capacity += c.chunk.size();
		if (c.replacement_id < args.size()) {
			capacity += args[c.replacement_id].size();
		}
	}

	ret.reserve(capacity);

	for (const auto& c : fmt) {
		ret.append(c.chunk);

		if (c.replacement_id < args.size()) {
			ret.append(args[c.replacement_id]);
		}
	}

	return ret;
}
