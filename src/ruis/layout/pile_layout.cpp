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

#include "pile_layout.hpp"

#include "../util/util.hpp"
#include "../widget/widget.hpp"

using namespace ruis;

void pile_layout::lay_out(const vector2& dims, semiconst_widget_list& widgets) const
{
	for (const auto& widget : widgets) {
		auto& w = widget.get();
		w.resize(dims_for_widget(w, dims));

		ruis::vector2 pos;
		for (unsigned i = 0; i != 2; ++i) {
			const auto& lp = w.get_layout_params_const();

			auto align = lp.align[i];
			switch (align) {
				case align::front:
					pos[i] = 0;
					break;
				case align::undefined:
					[[fallthrough]];
				case align::center:
					pos[i] = (dims[i] - w.rect().d[i]) / 2;
					break;
				case align::back:
					pos[i] = dims[i] - w.rect().d[i];
					break;
			}
		}

		using std::round;
		pos = round(pos);

		w.move_to(pos);
	}
}

vector2 pile_layout::measure(const vector2& quotum, const_widget_list& widgets) const
{
	vector2 ret(quotum);
	using std::max;
	ret = max(ret, real(0)); // clamp bottom

	for (const auto& w : widgets) {
		auto& lp = w.get().get_layout_params_const();

		ruis::vector2 d;

		for (unsigned j = 0; j != d.size(); ++j) {
			switch (lp.dims[j].get_type()) {
				case dim::max:
					if (quotum[j] >= 0) {
						d[j] = quotum[j];
					} else {
						d[j] = -1;
					}
					break;
				case dim::undefined:
					[[fallthrough]];
				case dim::min:
					d[j] = -1;
					break;
				case dim::fill:
					if (quotum[j] >= 0) {
						d[j] = quotum[j];
					} else {
						d[j] = 0;
					}
					break;
				case dim::length:
					d[j] = lp.dims[j].get_length().get(w.get().context);
					break;
			}
		}

		d = w.get().measure(d);

		for (unsigned j = 0; j != d.size(); ++j) {
			if (quotum[j] < 0) {
				using std::max;
				ret[j] = max(ret[j], d[j]); // clamp bottom
			}
		}
	}

	return ret;
}
