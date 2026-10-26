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

#include "scroll_bar.hpp"

#include <algorithm>

#include "../../../context.hpp"
#include "../../../util/corner_radius.hpp"

using namespace ruis;
using namespace ruis::touch;

scroll_bar::scroll_bar( //
	const utki::shared_ref<ruis::context>& context,
	all_parameters params
) :
	widget( //
		context,
		[&]() {
			// if the thin (transverse) dimension is not set to a concrete value (undefined or 'min')
			// then default it to 7pp (the style's 'len_gap' of 10pp reduced by a factor of 1.5 and rounded)
			const auto thickness = ruis::length::make_pp(7);
			auto& d = params.layout_params.dims;
			if (params.params.orientation.vertical) {
				if (ruis::dim::is_min_type(d.x().get_type())) {
					d.x() = thickness;
				}
			} else {
				if (ruis::dim::is_min_type(d.y().get_type())) {
					d.y() = thickness;
				}
			}
			return std::move(params.layout_params);
		}(),
		std::move(params.widget)
	),
	fraction_band_widget( //
		context,
		{
			.fraction = params.params.fraction,
			.band_fraction = params.params.band_fraction //
		}
	),
	oriented(std::move(params.params.orientation)),
	params([&]() {
		auto& p = params.params.specific;
		// resolve the handle color when undefined (the background color is left
		// as-is: when it is undefined the background track is simply not drawn)
		if (p.handle_color.get().is_undefined()) {
			p.handle_color = context.get().style().get_color_text();
		}
		return std::move(params.params);
	}()),
	vao(context.get().renderer, {}),
	fade_timeout_timer( //
		utki::make_shared<ruis::timer>( //
			context.get().updater, //
			[this](uint32_t) {
				this->on_delay_expire();
			} //
		) //
	),
	fade_animation( //
		utki::make_shared<ruis::animation>( //
			context.get().updater, //
			params.params.specific.fade_duration_ms //
		) //
	)
{
	// the fade-out animation linearly decreases the indicator's alpha from 1 to 0
	this->fade_animation.get().update_handler = [this](real factor) {
		this->fade_factor = factor;
		this->clear_cache(); // re-render with the new alpha
	};
	this->fade_animation.get().end_handler = [this](uint32_t) {
		// the fade-out animation is complete, become not-drawn again
		this->cur_state = state::hidden;
		this->clear_cache();
	};
}

scroll_bar::~scroll_bar()
{
	this->fade_animation.get().stop();
	this->fade_timeout_timer.get().stop();
}

void scroll_bar::on_fraction_change()
{
	this->fraction_widget::on_fraction_change();
	this->on_scroll_indicator_change();
}

void scroll_bar::on_band_change()
{
	this->fraction_band_widget::on_band_change();
	this->on_scroll_indicator_change();
}

void scroll_bar::on_resize()
{
	this->widget::on_resize();

	const auto w = this->rect().d.x();
	const auto h = this->rect().d.y();

	ruis::corners<real> radii;
	if (this->params.specific.rounded_corners) {
		// seed all corners with the "very large" equal radius, then normalize against
		// the widget's dimensions so the rectangle becomes a capsule (stadium)
		radii = big_length_px;
		normalize_corner_radii(radii, w, h);
	} else {
		// sharp corners (no rounding)
		radii = 0;
	}

	this->vao.set({
		.corner_radii = radii, //
		.stroke_width = -1 // filled rectangle
	});
}

void scroll_bar::on_delay_expire()
{
	this->cur_state = state::fading;
	this->fade_factor = 0;
	// rewind() is required because animation::play() does not reset left_ms;
	// after a previous fade left_ms stays 0, so without rewind() the reused
	// animation would be considered already ended and would skip the fade
	this->fade_animation.get().rewind();
	this->fade_animation.get().play();
}

void scroll_bar::on_scroll_indicator_change()
{
	// become visible (fully opaque)
	this->cur_state = state::waiting;
	this->fade_factor = 0;

	// cancel any in-progress fade-out animation
	this->fade_animation.get().stop();

	// (re)start the delay timer
	if (this->fade_timeout_timer.get().is_running()) {
		this->fade_timeout_timer.get().stop();
	}
	this->fade_timeout_timer.get().start(this->params.specific.fade_timeout_ms);

	// make the indicator visible right away
	this->clear_cache();
}

real scroll_bar::current_alpha() const
{
	switch (this->cur_state) {
		using enum state;
		case hidden:
			return 0;
		case waiting:
			return 1;
		case fading:
			return real(1) - this->fade_factor;
	}
	return 0;
}

color scroll_bar::scale_alpha(const color& c, real f)
{
	color result(c);
	// scale the alpha channel (index 3) by the factor [0:1], rounding to the
	// nearest integer
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "0.5 for rounding to the nearest integer")
	result[3] = static_cast<uint8_t>(real(c[3]) * f + real(0.5));
	return result;
}

void scroll_bar::render(const ruis::mat4& matrix) const
{
	if (this->cur_state == state::hidden) {
		return; // not drawn
	}

	const auto f = this->current_alpha();

	const auto& d = this->rect().d;
	const auto w = d.x();
	const auto h = d.y();

	// the background (track) capsule, it fills the whole widget;
	// it is only drawn when its color is defined (undefined by default, in which
	// case the background is not drawn at all)
	if (!this->params.specific.background_color.get().is_undefined()) {
		this->vao.render( //
			matrix, //
			{w, h}, //
			scroll_bar::scale_alpha(this->params.specific.background_color.get(), f)
		);
	}

	// the handle capsule
	{
		const unsigned long_index = this->get_long_index();
		const auto short_dim = d[this->get_trans_index()];
		const auto total_long = d[long_index];

		using std::max;

		// the handle length follows the band fraction, clamped so it is at least a
		// circle (the short dimension) to keep a valid capsule
		const auto handle_long = max(total_long * this->get_band_fraction(), short_dim);

		ruis::vec2 hdims(short_dim);
		hdims[long_index] = handle_long;

		const auto effective_length = max(total_long - handle_long, real(0));
		ruis::vec2 hoff(0);
		hoff[long_index] = effective_length * this->get_fraction();

		ruis::mat4 m(matrix);
		m.translate(hoff);
		this->vao.render( //
			m, //
			hdims, //
			scroll_bar::scale_alpha(this->params.specific.handle_color.get(), f)
		);
	}
}

utki::shared_ref<ruis::touch::scroll_bar> ruis::touch::make::scroll_bar( //
	const utki::shared_ref<ruis::context>& context,
	ruis::touch::scroll_bar::all_parameters params
)
{
	return utki::make_shared<ruis::touch::scroll_bar>(
		context, //
		std::move(params)
	);
}
