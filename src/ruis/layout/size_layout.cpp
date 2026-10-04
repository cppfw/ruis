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
		// 'min' (and 'undefined') children are measured against the space actually available
		// to them (from their position to the edge of the parent quotum).
		auto d = dims_for_widget(
			ww, //
			max( //
				quotum - ww.rect().p, //
				vec2(0) //
			), //
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
		auto& ww = w.get();
		if (ww.is_layout_dirty()) {
			using std::max;
			// 'min' (and 'undefined') children are not clamped to the space available, the content
			// is allowed to exceed the parent size (absolute positioning semantics),
			// so an infinite parent dimension is passed in those directions
			vec2 pd = max(dims - ww.rect().p, vec2(0));
			const auto& lp = ww.get_layout_params_const();
			if (dim::is_min_type(lp.dims[0].get_type())) {
				pd[0] = measure_infinite_quotum;
			}
			if (dim::is_min_type(lp.dims[1].get_type())) {
				pd[1] = measure_infinite_quotum;
			}

			ww.resize(dims_for_widget(ww, pd, r4::vector2<measure_mode>(measure_mode::exactly)));
		}
	}
}
