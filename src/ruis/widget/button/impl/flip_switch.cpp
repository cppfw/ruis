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

#include "flip_switch.hpp"

#include <algorithm>

#include "../../../context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace ruis::length_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

void flip_switch::on_pressed_change()
{
	this->update_knob();
	this->clear_cache();
	this->toggle_button::on_pressed_change();
}

void flip_switch::on_lay_out()
{
	this->capsule::on_lay_out();
	this->update_knob();
}

void flip_switch::update_knob()
{
	using std::max;

	// update the knob's color
	this->knob.set_color(this->is_pressed() ? this->params.knob_color_on : this->params.knob_color_off);

	// update the knob's position and size
	auto borders = this->get_actual_borders();
	auto content_dims = max(real(0), this->rect().d - borders.dims());
	auto d = max(real(0), content_dims[1]);

	this->knob.resize({d, d});
	this->knob.move_to(
		{this->is_pressed() ? content_dims[0] - d : 0, //
		 0}
	);
}

ruis::vec2 flip_switch::measure(
	const vec2& quotum, //
	const r4::vector2<measure_mode>& mode
) const
{
	const auto h = this->params.height.get().get(this->context);

	vec2 ret = {2 * h, h};

	for (unsigned i = 0; i != ret.size(); ++i) {
		if (mode[i] == measure_mode::exactly) {
			ret[i] = quotum[i];
		} else {
			using std::min;
			ret[i] = min(ret[i], quotum[i]); // clamp to the quotum
		}
	}

	return ret;
}

flip_switch::flip_switch(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	button( //
		context,
		std::move(params.button)
	),
	toggle_button(context),
	// clang-format off
	capsule(
		context,
		{
			.params = [&]() {
				auto& p = params.params.capsule;

				if (auto& l = p.specific.stroke_width; l.get().is_undefined()) {
					l = context.get().style().get_len_border();
				}
				if (auto& c = p.specific.stroke_color; c.get().is_undefined()) {
					c = context.get().style().get_color_primary();
				}
				if (auto& c = p.specific.fill_color; c.get().is_undefined()) {
					c = context.get().style().get_color_secondary();
				}

				for (auto& b : p.padding.specific.borders) {
					if (b.get().is_undefined()) {
						b = context.get().style().get_len_gap_small();
					}
				}

				return std::move(p);
			}()
		},
		{
			// the knob
			m::ellipse(
				context,
				{
					.layout_params{
						.dims = {ruis::dim::min, ruis::dim::fill}
					},
					.widget{
						.id = "ruis_flip_switch_knob"s
					},
					.params{
						.padding{
							.specific{
								.borders = ruis::styled<ruis::length>(0_pp)
							}
						},
						.color = ruis::color::transparent
					}
				}
			)
		}
	),
	knob(this->get_container().get_widget_as<ellipse>("ruis_flip_switch_knob"sv)),
	params([&]() {
		auto& p = params.params.specific;

		if (auto& l = p.height; l.get().is_undefined()) {
			l = 25_pp;
		}
		if (auto& c = p.knob_color_on; c.get().is_undefined()) {
			c = context.get().style().get_color_special();
		}
		if (auto& c = p.knob_color_off; c.get().is_undefined()) {
			c = context.get().style().get_color_primary();
		}

		return std::move(p);
	}())
{
	this->update_knob();
}
