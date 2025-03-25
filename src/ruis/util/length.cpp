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

#include "length.hpp"

using namespace std::string_view_literals;

using namespace ruis;

real length::get_internal(const ruis::units& units) const noexcept
{
	switch (this->value_units) {
		default:
			ASSERT(false)
			[[fallthrough]];
		case units::px:
			return this->value;
		case units::pp:
			return units.pp_to_px(this->value);
		case units::mm:
			return units.mm_to_px(this->value);
	}
}

std::ostream& ruis::operator<<(std::ostream& o, const ruis::length& l)
{
	if (l.is_undefined()) {
		return o << "undefiend";
	}

	o << l.value;

	switch (l.value_units) {
		case length::units::px:
			return o << "px";
		case length::units::pp:
			return o << "pp";
		case length::units::mm:
			return o << "mm";
	}

	return o;
}

length length::parse_style_value(const tml::forest& desc)
{
	if (desc.empty()) {
		return length::default_value();
	}

	utki::string_parser parser(desc.front().value.string);

	auto num = parser.read_number<ruis::real>();

	if (parser.empty()) {
		return length::make_px(num);
	}

	auto unit = parser.read_chars(std::numeric_limits<size_t>::max());

	if (unit == "pp"sv) {
		return length::make_pp(num);
	} else if (unit == "mm"sv) {
		return length::make_mm(num);
	}
	throw std::invalid_argument(utki::cat("length::parse_style_value(desc): unknown units: ", unit));
}
