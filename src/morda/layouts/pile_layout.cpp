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

#include "pile_layout.hpp"

#include "../util/util.hpp"
#include "../widget.hpp"

using namespace morda;

const utki::shared_ref<pile_layout> pile_layout::instance = utki::make_shared<pile_layout>();

void pile_layout::lay_out(const vector2& size, semiconst_widget_list& widgets) const
{
	for (const auto& w : widgets) {
		w.get().resize(dims_for_widget(w.get(), size));

		using std::round;
		w.get().move_to(round((size - w.get().rect().d) / 2));
	}
}

vector2 pile_layout::measure(const vector2& quotum, const_widget_list& widgets) const
{
	vector2 ret(quotum);
	using std::max;
	ret = max(ret, real(0)); // clamp bottom

	for (const auto& w : widgets) {
		auto& lp = w.get().get_layout_params_const();

		morda::vector2 d;

		for (unsigned j = 0; j != d.size(); ++j) {
			if (lp.dims[j] == layout_params::max) {
				if (quotum[j] >= 0) {
					d[j] = quotum[j];
				} else {
					d[j] = -1;
				}
			} else if (lp.dims[j] == layout_params::min) {
				d[j] = -1;
			} else if (lp.dims[j] == layout_params::fill) {
				if (quotum[j] >= 0) {
					d[j] = quotum[j];
				} else {
					d[j] = 0;
				}
			} else {
				d[j] = lp.dims[j];
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
