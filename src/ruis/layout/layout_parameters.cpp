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

#include "layout_parameters.hpp"

#include "../util/util.hpp"

using namespace ruis;

layout_parameters layout_parameters::make(const tml::forest& desc, const ruis::units& units)
{
	layout_parameters ret;
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		try {
			if (p.value == "dx") {
				ret.dims.x() = parse_layout_dimension_value(get_property_value(p), units);
			} else if (p.value == "dy") {
				ret.dims.y() = parse_layout_dimension_value(get_property_value(p), units);
			} else if (p.value == "weight") {
				ret.weight = get_property_value(p).to_float();
			}
		} catch (std::invalid_argument&) {
			LOG([&](auto& o) {
				o << "could not parse value of " << tml::to_string(p) << std::endl;
			})
			throw;
		}
	}
	return ret;
}
