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

#include "../../../animation/eased_animation.hpp"
#include "../../../context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace ruis::length_literals;

using namespace ruis;
using std::max;

namespace m {
using namespace ruis::make;
} // namespace m

void flip_switch::on_pressed_change()
{
	// the pressed (logical) state has already been flipped by set_pressed() before
	// this override was called; now animate the knob's visual progress from its
	// current position to the new target (1 when on, 0 when off). If the user clicks
	// again mid-animation this simply retargets from wherever the knob currently is.
	this->knob_animation.get().stop(); // cancel any in-flight animation

	this->from_progress = this->progress;
	this->to_progress = this->is_pressed() ? real(1) : real(0);

	this->clear_cache();

	// rewind() is required because animation::play() does not reset the animation's
	// remaining time; without it a reused animation would be considered already ended
	this->knob_animation.get().rewind();
	this->knob_animation.get().play();

	this->toggle_button::on_pressed_change();
}

flip_switch::~flip_switch()
{
	// stop the knob animation so the updater does not call its handlers after this
	// widget (and the 'this' they capture) is gone
	this->knob_animation.get().stop();
}

void flip_switch::on_lay_out()
{
	this->rectangle::on_lay_out();

	// the knob's travel length and diameter depend only on the widget's size, so they
	// are computed here (on (re)layout) and the knob is resized here; during the
	// animation the knob is only moved and recolored
	auto borders = this->get_actual_borders();
	this->content_w = max(real(0), this->rect().d.x() - borders.dims().x());
	this->knob_d = max(real(0), this->rect().d.y() - borders.dims().y());
	this->knob.resize({this->knob_d, this->knob_d});

	// the knob's visual state snaps to the logical pressed state on (re)layout;
	// it is only animated in response to user input
	this->from_progress = this->is_pressed() ? real(1) : real(0);
	this->to_progress = this->from_progress;
	this->progress = this->from_progress;
	this->apply_progress(this->progress);
}

real flip_switch::knob_x_at(real progress) const
{
	// the knob travels along the long axis from the left edge (off) to the right
	// edge (on) of the content area
	return (this->content_w - this->knob_d) * progress;
}

ruis::color flip_switch::knob_color_at(real progress) const
{
	return flip_switch::lerp(
		this->params.knob_color_off.get(), //
		this->params.knob_color_on.get(), //
		progress
	);
}

void flip_switch::apply_progress(real progress)
{
	// update the knob's color and position; its size is set once in on_lay_out and
	// only changes when the widget is resized
	this->knob.set_color(this->knob_color_at(progress));
	this->knob.move_to(
		{this->knob_x_at(progress), //
		 0}
	);
}

ruis::color flip_switch::lerp(const ruis::color& a, const ruis::color& b, real f)
{
	color c;
	for (size_t i = 0; i != c.size(); ++i) {
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "0.5 for rounding to the nearest integer")
		c[i] = static_cast<uint8_t>(real(a[i]) * (real(1) - f) + real(b[i]) * f + real(0.5));
	}
	return c;
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
	rectangle(
		context,
		{
			.params = [&]() {
				auto& p = params.params.rectangle;

				// give the rectangle a very large corner radius so that it looks like
				// a capsule (stadium); the radius is normalized to the widget's size
				// by the rectangle widget (see normalize_corner_radii)
				for (auto& r : p.specific.corner_radii) {
					if (r.get().is_undefined()) {
						r = length::make_px(big_length_px);
					}
				}
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
			constexpr auto default_height = 25_pp;
			l = default_height;
		}
		if (auto& c = p.knob_color_on; c.get().is_undefined()) {
			c = context.get().style().get_color_special();
		}
		if (auto& c = p.knob_color_off; c.get().is_undefined()) {
			c = context.get().style().get_color_primary();
		}

		return std::move(p);
	}()),
	knob_animation( //
		utki::make_shared<ruis::eased_animation>( //
			context.get().updater, //
			// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "200 ms knob animation duration")
			200, //
			[](real f) { return ruis::easing::out_cubic(f); } //
		) //
	)
{
	// animate the knob's visual progress towards the logical pressed state
	this->knob_animation.get().update_handler = [this](real factor) {
		this->progress = this->from_progress + (this->to_progress - this->from_progress) * factor;
		this->apply_progress(this->progress);
		this->clear_cache();
	};
	this->knob_animation.get().end_handler = [this](uint32_t) {
		// snap to the exact target on completion to avoid any floating-point drift
		this->progress = this->to_progress;
		this->apply_progress(this->progress);
		this->clear_cache();
	};
}
