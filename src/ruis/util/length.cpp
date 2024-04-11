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

#include "length.hpp"

using namespace ruis;

real length::get_internal(const context& ctx) const noexcept
{
	switch (this->value_units) {
		default:
			ASSERT(false)
			[[fallthrough]];
		case units::px:
			return this->value;
		case units::pp:
			return ctx.units.pp_to_px(this->value);
		case units::mm:
			return ctx.units.mm_to_px(this->value);
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
