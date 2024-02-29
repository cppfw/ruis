/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "button.hpp"

namespace ruis {

/**
 * @brief Basic class of a push button.
 * Push button is a button which can be clicked.
 * Clicked means that the button was pressed and then released.
 */
class push_button : virtual public button
{
	bool is_currently_pressed = false;

	unsigned pointer_id = 0;

protected:
	void on_pressed_change() override;

	/**
	 * @brief Invoked when the button is clicked.
	 * Default implementation emits 'clicked' signal.
	 */
	virtual void on_click();

	bool on_mouse_button(const mouse_button_event& event) override;

	void on_hovered_change(unsigned pointer_id) override;

	push_button(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		button(this->context, desc)
	{}

	push_button( //
		utki::shared_ref<ruis::context> context,
		button::parameters button_params
	);

public:
	push_button(const push_button&) = delete;
	push_button& operator=(const push_button&) = delete;

	push_button(push_button&&) = delete;
	push_button& operator=(push_button&&) = delete;

	~push_button() override = default;

	/**
	 * @brief Clicked signal.
	 * Emitted when the button is clicked.
	 */
	std::function<void(push_button&)> click_handler;
};

} // namespace ruis
