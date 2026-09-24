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

#include "linear_layout.hpp"

#include <utki/config.hpp>

#include "../util/util.hpp"
#include "../widget/widget.hpp"

using namespace ruis;

linear_layout::linear_layout(bool is_vertical) :
	oriented({.vertical = is_vertical})
{}

namespace {
struct info {
	ruis::vec2 measured_dims;
};
} // namespace

void linear_layout::lay_out(
	const vec2& dims, //
	semiconst_widget_list& widgets
) const
{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(widgets.size());

	// calculate rigid size, net weight and store weights
	real rigid = 0;
	real net_weight = 0;

	{
		// TODO: use utki::zip(info, widgets)
		auto info = info_array.begin();
		for (const auto& w : widgets) {
			const auto& lp = w.get().get_layout_params_const();

			using std::max;
			real weight = max(real(0), lp.weight.get()); // clamp bottom

			net_weight += weight;

			const auto& trans_dim = lp.dims[trans_index];

			vec2 d;
			switch (trans_dim.get_type()) {
				case dim::type::max:
					[[fallthrough]];
				case dim::type::fill:
					d[trans_index] = dims[trans_index];
					break;
				case dim::type::undefined:
					[[fallthrough]];
				case dim::type::min:
					d[trans_index] = -1; // will be updated below
					break;
				case dim::type::length:
					d[trans_index] = trans_dim.get_length().get(w.get().context);
					break;
			}

			const auto& long_dim = lp.dims[long_index];

			switch (long_dim.get_type()) {
				case dim::type::fill:
					d[long_index] = 0;
					break;
				// NOLINTNEXTLINE(bugprone-branch-clone, "false positive")
				case dim::type::undefined:
					[[fallthrough]];
				case dim::type::min:
					[[fallthrough]];
				case dim::type::max:
					d[long_index] = -1; // will be updated below
					break;
				case dim::type::length:
					d[long_index] = long_dim.get_length().get(w.get().context);
					break;
			}

			if (!d.is_positive_or_zero()) {
				vec2 md = w.get().measure(d);
				for (unsigned i = 0; i != md.size(); ++i) {
					if (d[i] < 0) {
						d[i] = md[i];
					}
				}
			}

			info->measured_dims = d;

			rigid += d[long_index];

			++info;
		}
	}

	// arrange widgets
	{
		using std::round;

		real flexible = dims[long_index] - rigid;

		real pos = 0;

		real remainder = 0;

		// TODO: use utki::zip(info, widgets)
		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			using std::max;
			real weight = max(real(0), lp.weight.get()); // clamp bottom

			auto long_room = info->measured_dims[long_index];

			if (weight != 0) {
				utki::assert(weight > 0);
				vec2 d;
				if (flexible > 0) {
					utki::assert(net_weight > 0);
					real dl = flexible * weight / net_weight;
					real floored = std::floor(dl);
					utki::assert(dl >= floored);
					long_room += floored;
					remainder += (dl - floored);
					if (remainder >= real(1)) {
						long_room += real(1);
						remainder -= real(1);
					}
				}

				const auto& long_dim = lp.dims[long_index];

				switch (long_dim.get_type()) {
					case dim::type::fill:
						[[fallthrough]];
					case dim::type::max:
						d[long_index] = long_room;
						break;
					// NOLINTNEXTLINE(bugprone-branch-clone, "false positive")
					case dim::type::undefined:
						[[fallthrough]];
					case dim::type::min:
						[[fallthrough]];
					case dim::type::length:
						d[long_index] = info->measured_dims[long_index];
						break;
				}

				const auto& trans_dim = lp.dims[trans_index];

				switch (trans_dim.get_type()) {
					case dim::type::max:
						[[fallthrough]];
					case dim::type::fill:
						d[trans_index] = dims[trans_index];
						break;
					case dim::type::undefined:
						[[fallthrough]];
					case dim::type::min:
						d[trans_index] = -1;
						break;
					case dim::type::length:
						d[trans_index] = trans_dim.get_length().get(w.get().context);
						break;
				}
				switch (trans_dim.get_type()) {
					// NOLINTNEXTLINE(bugprone-branch-clone, "false positive")
					case dim::type::undefined:
						[[fallthrough]];
					case dim::type::min:
						[[fallthrough]];
					case dim::type::length:
						if (d.x() < 0 || d.y() < 0) {
							vec2 md = w.get().measure(d);
							for (unsigned i = 0; i != md.size(); ++i) {
								if (d[i] < 0) {
									d[i] = md[i];
								}
							}
						}
						break;
					case dim::type::max:
						[[fallthrough]];
					case dim::type::fill:
						break;
				}

				w.get().resize(d);
			} else {
				w.get().resize(info->measured_dims);
			}

			vec2 room;
			room[long_index] = long_room;
			room[trans_index] = dims[trans_index];

			vec2 new_pos;
			new_pos[long_index] = pos;
			new_pos[trans_index] = 0;

			for (unsigned i = 0; i != 2; ++i) {
				const auto& align = lp.align[i].get();
				switch (align) {
					case ruis::align::front:
						break;
					case ruis::align::undefined:
						[[fallthrough]];
					case ruis::align::center:
						new_pos[i] += round((room[i] - w.get().rect().d[i]) / 2);
						break;
					case ruis::align::back:
						new_pos[i] += room[i] - w.get().rect().d[i];
						break;
				}
			}

			w.get().move_to(new_pos);

			pos += long_room;
			++info;
		}

		utki::assert(remainder >= 0);
		utki::assert(remainder < real(1));

		// TODO: is it ok to always reisze last widget?
		if (remainder > 0) {
			vec2 d;
			d[trans_index] = 0;
			d[long_index] = round(remainder);
			widgets.back().get().resize_by(d);
		}
	}
}

ruis::vec2 linear_layout::measure(
	const vec2& quotum, //
	const_widget_list& widgets
) const
{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(widgets.size());

	// Pass 1: determine the max height (transverse size) by going through the
	// widgets which have non-fill transverse dimension and measuring them.
	real height = quotum[trans_index] >= 0 ? quotum[trans_index] : 0;
	real net_weight = 0;

	// TODO: use utki::zip(info, widgets)
	for (auto info = info_array.begin(); const auto& w : widgets) {
		auto& lp = w.get().get_layout_params_const();

		using std::max;
		real weight = max(real(0), lp.weight.get()); // clamp bottom

		net_weight += weight;

		const auto& trans_dim = lp.dims[trans_index];

		// fill transverse widgets do not determine the height, they will be measured
		// in the second pass with the final transverse size
		if (trans_dim.get_type() != dim::type::fill) {
			vec2 child_quotum;

			switch (trans_dim.get_type()) {
				case dim::type::max:
					if (quotum[trans_index] >= 0) {
						child_quotum[trans_index] = quotum[trans_index];
					} else {
						child_quotum[trans_index] = -1;
					}
					break;
				case dim::type::undefined:
					[[fallthrough]];
				case dim::type::min:
					child_quotum[trans_index] = -1;
					break;
				case dim::type::length:
					child_quotum[trans_index] = trans_dim.get_length().get(w.get().context);
					break;
				case dim::type::fill:
					break; // handled above
			}

			const auto& long_dim = lp.dims[long_index];

			switch (long_dim.get_type()) {
				// NOLINTNEXTLINE(bugprone-branch-clone, "false positive")
				case dim::type::undefined:
					[[fallthrough]];
				case dim::type::min:
					[[fallthrough]];
				case dim::type::max:
					child_quotum[long_index] = -1;
					break;
				case dim::type::fill:
					child_quotum[long_index] = 0;
					break;
				case dim::type::length:
					child_quotum[long_index] = long_dim.get_length().get(w.get().context);
					break;
			}

			info->measured_dims = w.get().measure(child_quotum);

			if (quotum[trans_index] < 0) {
				height = max(height, info->measured_dims[trans_index]);
			}
		}

		++info;
	}

	// Pass 2: measure all the fill and max widgets with the transverse size found in
	// the previous pass to get their longitudinal size (max widgets are measured again)
	{
		// TODO: use utki::zip(info, widgets)
		auto info = info_array.begin();
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			const auto t = lp.dims[trans_index].get_type();
			if (t == dim::type::fill || t == dim::type::max) {
				vec2 child_quotum;
				child_quotum[trans_index] = height;

				const auto& long_dim = lp.dims[long_index];
				switch (long_dim.get_type()) {
					// NOLINTNEXTLINE(bugprone-branch-clone, "false positive")
					case dim::type::undefined:
						[[fallthrough]];
					case dim::type::min:
						[[fallthrough]];
					case dim::type::max:
						child_quotum[long_index] = -1;
						break;
					case dim::type::fill:
						child_quotum[long_index] = 0;
						break;
					case dim::type::length:
						child_quotum[long_index] = long_dim.get_length().get(w.get().context);
						break;
				}

				info->measured_dims = w.get().measure(child_quotum);
			}

			++info;
		}
	}

	// rigid length is the sum of the longitudinal sizes of all the widgets
	real rigid_length = 0;
	for (const auto& info : info_array) {
		rigid_length += info.measured_dims[long_index];
	}

	vec2 ret;

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
		auto last_child = widgets.size() != 0 ? &widgets.back().get() : nullptr;
		auto info = info_array.begin();
		real remainder = 0;
		// TODO: use utki::zip(info, widgets)
		for (const auto& w : widgets) {
			auto& lp = w.get().get_layout_params_const();

			using std::max;
			real weight = max(real(0), lp.weight.get()); // clamp bottom

			if (weight == 0) {
				continue;
			}

			vec2 d;
			d[long_index] = info->measured_dims[long_index];

			if (flex_len > 0) {
				using std::floor;

				utki::assert(net_weight > 0);

				real dl = flex_len * weight / net_weight;
				real floored = floor(dl);
				utki::assert(dl >= floored);
				d[long_index] += floored;
				remainder += (dl - floored);
				if (remainder >= real(1)) {
					d[long_index] += real(1);
					remainder -= real(1);
				}
				if (&w.get() == last_child) {
					if (remainder > 0) {
						using std::round;

						vec2 correction;
						correction[trans_index] = 0;
						correction[long_index] = round(remainder);
						d += correction;
					}
				}
			}

			const auto& trans_dim = lp.dims[trans_index];

			switch (trans_dim.get_type()) {
				case dim::type::max:
					if (quotum[trans_index] >= 0) {
						d[trans_index] = quotum[trans_index];
					} else {
						d[trans_index] = -1;
					}
					break;
				case dim::type::undefined:
					[[fallthrough]];
				case dim::type::min:
					d[trans_index] = -1;
					break;
				case dim::type::fill:
					if (quotum[trans_index] >= 0) {
						d[trans_index] = quotum[trans_index];
					} else {
						d[trans_index] = 0;
					}
					break;
				case dim::type::length:
					d[trans_index] = trans_dim.get_length().get(w.get().context);
					break;
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
