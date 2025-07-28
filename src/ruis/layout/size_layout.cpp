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

#include "size_layout.hpp"

#include "../util/util.hpp"
#include "../widget/widget.hpp"

using namespace ruis;

void size_layout::lay_out(
	const vector2& dims, //
	semiconst_widget_list& widgets
) const
{
	for (const auto& w : widgets) {
		if (w.get().is_layout_dirty()) {
			using std::max;
			auto d = dims_for_widget(
				w.get(), //
				max(
					dims - w.get().rect().p,
					{0, 0} //
				)
			);
			w.get().resize(d);
		}
	}
}
