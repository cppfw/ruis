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

#pragma once

#include "../../container.hpp"
#include "../choice_button.hpp"

namespace ruis {

/**
 * @brief Radio button widget.
 * Radio button is a choice button which displays a thick dot when checked and
 * an empty round when unchecked. In GUI script it can be instantiated as
 * "radio_button". Only available after initializing standard ruis widgets.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via choice_button")
class radio_button :
	virtual public choice_button, //
	private container
{
	const utki::shared_ref<widget> check_widget;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		button::parameters button_params;
	};

	radio_button(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	radio_button(const radio_button&) = delete;
	radio_button& operator=(const radio_button&) = delete;

	radio_button(radio_button&&) = delete;
	radio_button& operator=(radio_button&&) = delete;

	~radio_button() override = default;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->choice_button::on_mouse_button(e);
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		this->choice_button::on_hovered_change(pointer_id);
	}

	void on_pressed_change() override;

private:
};

namespace make {
inline utki::shared_ref<ruis::radio_button> radio_button(
	utki::shared_ref<ruis::context> context, //
	ruis::radio_button::all_parameters params
)
{
	return utki::make_shared<ruis::radio_button>(
		std::move(context), //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
