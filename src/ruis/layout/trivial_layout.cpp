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

#include "trivial_layout.hpp"

#include "../widget/widget.hpp"

using namespace ruis::layout;

ruis::vec2 trivial_layout::measure(
	const vec2& quotum, //
	const_widget_list& widgets
) const
{
	if (quotum.is_positive_or_zero()) {
		return quotum;
	}

	vec2 max_extent(0, 0);

	for (const auto& widget : widgets) {
		const auto& w = widget.get();

		auto extent = w.rect().x2_y2();

		using std::max;
		max_extent = max(extent, max_extent);
	}

	vec2 ret;
	for (auto [r, q, me] : utki::views::zip(ret, quotum, max_extent)) {
		if (q < 0) {
			r = me;
		} else {
			r = q;
		}
	}

	return ret;
}

void trivial_layout::lay_out(
	const vec2& dims, //
	semiconst_widget_list& widgets
) const
{
	for (auto& w : widgets) {
		auto& ww = w.get();
		if (ww.is_layout_dirty()) {
			ww.lay_out();
		}
	}
}
