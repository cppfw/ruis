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

#pragma once

#include "../../../animation/eased_animation.hpp"
#include "../../label/ellipse.hpp"
#include "../../label/rectangle.hpp"
#include "../toggle_button.hpp"

namespace ruis {

/**
 * @brief Flip switch widget.
 * Flip switch is a toggle button which looks like a horizontal capsule with a
 * circular knob inside of it. The knob is at the left position of the capsule
 * when the switch is off and at the right position when the switch is on.
 * The capsule has a border of the style's primary color with the style's
 * 'len_border' thickness and a background of the style's secondary color.
 * The knob is padded from the capsule's edges by the style's 'len_gap_small'
 * length. The knob is of the style's special color when the switch is on and
 * of the style's primary color when the switch is off.
 * The natural size of the flip switch is 25pp in height and twice that in width.
 */
// TODO: is this lint suppression needed?
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via toggle_button")

class flip_switch :
	public toggle_button, //
	public rectangle
{
	// TODO: refactor to use utki::shared_ref
	// NOLINTNEXTLINE(clang-analyzer-webkit.NoUncountedMemberChecker, "false-positive")
	ellipse& knob;

public:
	struct specific_parameters {
		/**
		 * @brief The natural height of the flip switch.
		 * The natural width is twice the height.
		 * If undefined then 25pp is used.
		 */
		styled<length> height;

		/**
		 * @brief The color of the knob when the switch is on.
		 * If undefined then the style's 'color_special' is used.
		 */
		styled<ruis::color> knob_color_on;

		/**
		 * @brief The color of the knob when the switch is off.
		 * If undefined then the style's 'color_primary' is used.
		 */
		styled<ruis::color> knob_color_off;
	};

	struct parameters {
		/**
		 * @brief The rectangle parameters. The rectangle is drawn with a very large
		 * corner radius so that it looks like a capsule (stadium). Its padding is the
		 * gap between the rectangle's edges and the knob.
		 * If the padding is undefined then the style's 'len_gap_small' is used.
		 */
		ruis::rectangle::parameters rectangle;
		specific_parameters specific;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		ruis::button::parameters button;
		parameters params;
	};

	flip_switch(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	flip_switch(const flip_switch&) = delete;
	flip_switch& operator=(const flip_switch&) = delete;

	flip_switch(flip_switch&&) = delete;
	flip_switch& operator=(flip_switch&&) = delete;

	~flip_switch() override;

	event_status on_mouse_button(const mouse_button_event& e) override
	{
		return this->toggle_button::on_mouse_button(e);
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		this->toggle_button::on_hovered_change(pointer_id);
	}

	void on_pressed_change() override;

	void on_lay_out() override;

	ruis::vec2 measure(
		const vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const override;

private:
	specific_parameters params;

	// the length of the knob's travel along the long axis (content width) and the
	// knob's diameter; both depend only on the widget's size, so they are computed
	// and stored in on_lay_out rather than on every animation frame
	real content_w = 0;
	real knob_d = 0;

	// visual progress of the knob (0 = off/left, 1 = on/right); it animates
	// towards the logical pressed state, independent of it
	real from_progress = 0;
	real to_progress = 0;
	real progress = 0;

	// animates the knob's position and color with easing; owned by a shared_ptr
	// because animation::play() relies on shared_from_this()
	utki::shared_ref<ruis::eased_animation> knob_animation;

	// the x offset of the left edge of the knob for a given knob progress
	real knob_x_at(real progress) const;
	// the knob color for a given knob progress (lerp between off and on colors)
	ruis::color knob_color_at(real progress) const;
	// apply a given knob progress to the knob's position and color
	void apply_progress(real progress);
	// linearly interpolate two colors (factor is in [0,1])
	static ruis::color lerp(const ruis::color& a, const ruis::color& b, real f);
};

namespace make {
inline utki::shared_ref<ruis::flip_switch> flip_switch( //
	const utki::shared_ref<ruis::context>& context,
	flip_switch::all_parameters params
)
{
	return utki::make_shared<ruis::flip_switch>( //
		context,
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
