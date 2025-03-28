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

#include "dimension.hpp"

using namespace std::string_view_literals;

using namespace ruis;

const dimension dimension::min(dimension::type::min);
const dimension dimension::max(dimension::type::max);
const dimension dimension::fill(dimension::type::fill);

dimension dimension::make_from(const tml::forest& desc)
{
	if (desc.empty()) {
		return {};
	}

	const auto& s = desc.front().value.string;
	if (s == "min"sv) {
		return dim::min;
	} else if (s == "fill"sv) {
		return dim::fill;
	} else if ((s == "max"sv)) {
		return dim::max;
	}

	return length::make_from(desc);
}

std::ostream& ruis::operator<<(std::ostream& o, const dimension& d)
{
	switch (d.get_type()) {
		case dimension::type::fill:
			o << "fill"sv;
			break;
		case dimension::type::min:
			o << "min"sv;
			break;
		case dimension::type::max:
			o << "max"sv;
			break;
		case dimension::type::length:
			o << d.get_length();
			break;
		case dimension::type::undefined:
			o << "undefined"sv;
			break;
	}
	return o;
}