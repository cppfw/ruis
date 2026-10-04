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

#include "pile_layout.hpp"

#include "../util/util.hpp"
#include "../widget/widget.hpp"

using namespace ruis;

void pile_layout::lay_out(
	const vec2& dims, //
	semiconst_widget_list& widgets
) const
{
	for (const auto& widget : widgets) {
		auto& w = widget.get();
		w.resize(w.measure_within_parent(
			dims, //
			r4::vector2<measure_mode>(measure_mode::exactly)
		));

		ruis::vec2 pos;
		for (unsigned i = 0; i != 2; ++i) {
			const auto& lp = w.get_layout_params_const();

			const auto& align = lp.align[i];
			switch (align.get()) {
				case ruis::align::front:
					pos[i] = 0;
					break;
				case ruis::align::undefined:
					[[fallthrough]];
				case ruis::align::center:
					pos[i] = (dims[i] - w.rect().d[i]) / 2;
					break;
				case ruis::align::back:
					pos[i] = dims[i] - w.rect().d[i];
					break;
			}
		}

		using std::round;
		pos = round(pos);

		w.move_to(pos);
	}
}

ruis::vec2 pile_layout::measure(
	const vec2& quotum, //
	const r4::vector2<measure_mode>& mode, //
	const_widget_list& widgets
) const
{
	vec2 ret;
	for (unsigned j = 0; j != ret.size(); ++j) {
		ret[j] = (mode[j] == measure_mode::exactly) ? quotum[j] : real(0);
	}

	for (const auto& w : widgets) {
		auto& lp = w.get().get_layout_params_const();

		// A child with fill dims in both directions does not constrain the pile's min size:
		// it fills whatever space it is given, so there is no point in measuring it.
		if (lp.dims[0].get_type() == ruis::dim::type::fill && lp.dims[1].get_type() == ruis::dim::type::fill) {
			continue;
		}

		ruis::vec2 d;
		r4::vector2<measure_mode> child_mode;

		// TODO: use utki::zip for d, lp.dims, child_mode?
		for (unsigned j = 0; j != d.size(); ++j) {
			const auto& dim = lp.dims[j];

			switch (dim.get_type()) {
				case ruis::dim::type::max:
					// we know the child will be resized to the parent size for max when layouting,
					// so if the parent is measured exactly, the child is measured with exactly
					// the parent size, otherwise it behaves the same as 'min'
					d[j] = quotum[j];
					child_mode[j] = mode[j];
					break;
				case ruis::dim::type::undefined:
					[[fallthrough]];
				case ruis::dim::type::min:
					// the child is measured as at_most against the quotum, so it reports its size
					// clamped to the available space
					d[j] = quotum[j];
					child_mode[j] = measure_mode::at_most;
					break;
				case ruis::dim::type::fill:
					if (mode[j] == measure_mode::exactly) {
						d[j] = quotum[j];
					} else {
						d[j] = 0;
					}
					child_mode[j] = measure_mode::exactly;
					break;
				case ruis::dim::type::length:
					d[j] = dim.get_length().get(w.get().context);
					child_mode[j] = measure_mode::exactly;
					break;
			}
		}

		d = w.get().measure(d, child_mode);

		for (unsigned j = 0; j != d.size(); ++j) {
			if (mode[j] == measure_mode::at_most) {
				using std::max;
				ret[j] = max(ret[j], d[j]); // clamp bottom
			}
		}
	}

	return ret;
}
