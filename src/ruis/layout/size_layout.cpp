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

#include "size_layout.hpp"

#include "../util/util.hpp"
#include "../widget/widget.hpp"

using namespace ruis;

ruis::vec2 size_layout::measure(
	const vec2& quotum, //
	const r4::vector2<measure_mode>& mode, //
	const_widget_list& widgets
) const
{
	ruis::vec2 ret;
	for (unsigned i = 0; i != ret.size(); ++i) {
		ret[i] = (mode[i] == measure_mode::exactly) ? quotum[i] : ruis::real(0);
	}

	for (const auto& w : widgets) {
		auto& ww = w.get();
		// Resolve the child's size according to its layout parameters.
		auto d = dims_for_widget(
			ww, //
			vec2(0), //
			r4::vector2<measure_mode>(measure_mode::at_most)
		);
		for (unsigned i = 0; i != ret.size(); ++i) {
			if (mode[i] == measure_mode::at_most) {
				using std::max;
				ret[i] = max(ret[i], ww.rect().p[i] + d[i]);
			}
		}
	}

	return ret;
}

void size_layout::lay_out(
	const vec2& dims, //
	semiconst_widget_list& widgets
) const
{
	for (const auto& w : widgets) {
		if (w.get().is_layout_dirty()) {
			using std::max;
			auto d = dims_for_widget(
				w.get(), //
				max( //
					dims - w.get().rect().p,
					{0, 0} //
				), //
				r4::vector2<measure_mode>(measure_mode::exactly)
			);
			w.get().resize(d);
		}
	}
}
