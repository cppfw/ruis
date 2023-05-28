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

#include "ratio_proxy.hpp"

using namespace morda;

ratio_proxy::ratio_proxy(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	morda::widget(c, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "ratio") {
			this->x_above_y = get_property_value(p).to_float();
		}
	}
}

void ratio_proxy::set_aspect_ratio(real x_above_y)
{
	if (this->x_above_y == x_above_y) {
		return;
	}

	this->x_above_y = x_above_y;
	this->invalidate_layout();
}

morda::vector2 ratio_proxy::measure(const morda::vector2& quotum) const
{
	if (quotum.x() < 0 && quotum.y() < 0) {
		return {0};
	}

	if (quotum.x() >= 0 && quotum.y() < 0) {
		return morda::vector2(quotum.x(), quotum.x() / this->x_above_y);
	}

	return morda::vector2(quotum.y() * this->x_above_y, quotum.y());
}
