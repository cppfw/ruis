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

#include "trivial_layout.hpp"

#include "../widget/widget.hpp"

using namespace ruis;

vector2 trivial_layout::measure(const vector2& quotum, const_widget_list& widgets) const
{
	return max(quotum, 0);
}

void trivial_layout::lay_out(const vector2& dims, semiconst_widget_list& widgets) const
{
	for (auto& w : widgets) {
		auto& ww = w.get();
		if (ww.is_layout_dirty()) {
			ww.lay_out();
		}
	}
}
