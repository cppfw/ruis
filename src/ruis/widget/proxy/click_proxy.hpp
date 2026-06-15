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

#include "../widget.hpp"

namespace ruis {

class click_proxy : virtual public widget
{
	bool is_pressed_v = false;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
	};

	click_proxy(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	click_proxy(const click_proxy&) = delete;
	click_proxy& operator=(const click_proxy&) = delete;

	click_proxy(click_proxy&&) = delete;
	click_proxy& operator=(click_proxy&&) = delete;

	~click_proxy() override = default;

	event_status on_mouse_button(const mouse_button_event& event) override;

	void on_hovered_change(unsigned pointer_id) override;

	bool is_pressed() const noexcept
	{
		return this->is_pressed_v;
	}

	/**
	 * @brief Handler for mouse press state changes.
	 * The event is always consumed, because otherwise the mouse is not captured and we
	 * never receive the mbutton up event neede to detect a button click.
	 */
	std::function<void(click_proxy& w)> pressed_change_handler;

	/**
	 * @brief Handler for clicked event.
	 */
	std::function<void(click_proxy& w)> click_handler;
};

namespace make {
utki::shared_ref<ruis::click_proxy> click_proxy(
	utki::shared_ref<ruis::context> context, //
	ruis::click_proxy::all_parameters params
);
} // namespace make

} // namespace ruis
