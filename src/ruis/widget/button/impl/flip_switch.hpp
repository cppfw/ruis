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

#include "../../label/capsule.hpp"
#include "../../label/ellipse.hpp"
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
	public capsule
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
		 * @brief The capsule parameters. The capsule's padding is the gap between
		 * the capsule's edges and the knob.
		 * If the padding is undefined then the style's 'len_gap_small' is used.
		 */
		ruis::capsule::parameters capsule;
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

	~flip_switch() override = default;

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

	void update_knob();
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
