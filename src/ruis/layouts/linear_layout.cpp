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

#include "linear_layout.hpp"

#include "../util/util.hpp"
#include "../widget.hpp"

using namespace morda;

const utki::shared_ref<row_layout> row_layout::instance = utki::make_shared<row_layout>();
const utki::shared_ref<column_layout> column_layout::instance = utki::make_shared<column_layout>();

linear_layout::linear_layout(bool is_vertical) :
	oriented(is_vertical)
{}

namespace {
struct info {
	vector2 measured_dims;
};
} // namespace

void linear_layout::lay_out(const vector2& size, semiconst_widget_list& widgets) const
{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(widgets.size());

	// calculate rigid size, net weight and store weights
	real rigid = 0;
	real net_weight = 0;

	{
		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			net_weight += lp.weight;

			ASSERT(lp.dims[long_index] != layout_params::max)
			ASSERT(lp.dims[long_index] != layout_params::fill)

			vector2 d = dims_for_widget(w.get(), size);
			info->measured_dims = d;

			rigid += d[long_index];

			++info;
		}
	}

	// arrange widgets
	{
		real flexible = size[long_index] - rigid;

		real pos = 0;

		real remainder = 0;

		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			if (lp.weight != 0) {
				ASSERT(lp.weight > 0)
				vector2 d;
				d[long_index] = info->measured_dims[long_index];
				if (flexible > 0) {
					ASSERT(net_weight > 0)
					real dl = flexible * lp.weight / net_weight;
					real floored = std::floor(dl);
					ASSERT(dl >= floored)
					d[long_index] += floored;
					remainder += (dl - floored);
					if (remainder >= real(1)) {
						d[long_index] += real(1);
						remainder -= real(1);
					}
				}

				if (lp.dims[trans_index] == layout_params::max || lp.dims[trans_index] == layout_params::fill) {
					d[trans_index] = size[trans_index];
				} else {
					if (lp.dims[trans_index] == layout_params::min) {
						d[trans_index] = -1;
					} else {
						d[trans_index] = lp.dims[trans_index];
					}
					if (d.x() < 0 || d.y() < 0) {
						vector2 md = w.get().measure(d);
						for (unsigned i = 0; i != md.size(); ++i) {
							if (d[i] < 0) {
								d[i] = md[i];
							}
						}
					}
				}
				w.get().resize(d);
			} else {
				w.get().resize(info->measured_dims);
			}

			vector2 new_pos;

			new_pos[long_index] = pos;

			pos += w.get().rect().d[long_index];

			new_pos[trans_index] = std::round((size[trans_index] - w.get().rect().d[trans_index]) / 2);

			w.get().move_to(new_pos);

			++info;
		}

		if (remainder > 0) {
			vector2 d;
			d[trans_index] = 0;
			d[long_index] = std::round(remainder);
			widgets.back().get().resize_by(d);
			widgets.back().get().move_by(-d);
		}
	}
}

vector2 linear_layout::measure(const vector2& quotum, const_widget_list& widgets) const
{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(widgets.size());

	// calculate rigid length
	real rigid_length = 0;
	real height = quotum[trans_index] >= 0 ? quotum[trans_index] : 0;
	real net_weight = 0;

	{
		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			net_weight += lp.weight;

			if (lp.dims[long_index] == layout_params::max || lp.dims[long_index] == layout_params::fill) {
				throw std::logic_error(
					"linear_layout::measure(): 'max' or 'fill' in longitudional direction specified in layout parameters"
				);
			}

			vector2 child_quotum;
			if (lp.dims[trans_index] == layout_params::max) {
				if (quotum[trans_index] >= 0) {
					child_quotum[trans_index] = quotum[trans_index];
				} else {
					child_quotum[trans_index] = -1;
				}
			} else if (lp.dims[trans_index] == layout_params::min) {
				child_quotum[trans_index] = -1;
			} else if (lp.dims[trans_index] == layout_params::fill) {
				if (quotum[trans_index] >= 0) {
					child_quotum[trans_index] = quotum[trans_index];
				} else {
					child_quotum[trans_index] = 0;
				}
			} else {
				child_quotum[trans_index] = lp.dims[trans_index];
			}

			ASSERT(lp.dims[long_index] != layout_params::max)
			ASSERT(lp.dims[long_index] != layout_params::fill)
			if (lp.dims[long_index] == layout_params::min) {
				child_quotum[long_index] = -1;
			} else {
				child_quotum[long_index] = lp.dims[long_index];
			}

			info->measured_dims = w.get().measure(child_quotum);

			rigid_length += info->measured_dims[long_index];

			if (lp.weight == 0) {
				if (quotum[trans_index] < 0) {
					using std::max;
					height = max(height, info->measured_dims[trans_index]);
				}
			}

			++info;
		}
	}

	vector2 ret;

	auto flex_len = [&]() -> real {
		if (quotum[long_index] < 0) {
			ret[long_index] = rigid_length;
			return 0;
		} else {
			ret[long_index] = quotum[long_index];
			return quotum[long_index] - rigid_length;
		}
	}();

	{
		real remainder = 0;

		auto last_child = widgets.size() != 0 ? &widgets.back().get() : nullptr;

		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();
			ASSERT(lp.weight >= 0)
			if (lp.weight == 0) {
				continue;
			}

			ASSERT(net_weight > 0)

			vector2 d;
			d[long_index] = info->measured_dims[long_index];

			if (flex_len > 0) {
				real dl = flex_len * lp.weight / net_weight;
				real floored = std::floor(dl);
				ASSERT(dl >= floored)
				d[long_index] += floored;
				remainder += (dl - floored);
				if (remainder >= real(1)) {
					d[long_index] += real(1);
					remainder -= real(1);
				}
				if (&w.get() == last_child) {
					if (remainder > 0) {
						vector2 correction;
						correction[trans_index] = 0;
						correction[long_index] = std::round(remainder);
						d += correction;
					}
				}
			}

			if (lp.dims[trans_index] == layout_params::max) {
				if (quotum[trans_index] >= 0) {
					d[trans_index] = quotum[trans_index];
				} else {
					d[trans_index] = -1;
				}
			} else if (lp.dims[trans_index] == layout_params::min) {
				d[trans_index] = -1;
			} else if (lp.dims[trans_index] == layout_params::fill) {
				if (quotum[trans_index] >= 0) {
					d[trans_index] = quotum[trans_index];
				} else {
					d[trans_index] = 0;
				}
			} else {
				d[trans_index] = lp.dims[trans_index];
			}

			if (quotum[trans_index] < 0) {
				using std::max;
				height = max(height, w.get().measure(d)[trans_index]);
			}

			++info;
		}
	}

	ret[trans_index] = height;
	return ret;
}
