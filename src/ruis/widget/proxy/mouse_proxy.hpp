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

/**
 * @brief Widget for intercepting mouse events.
 * From GUI script it can be instantiated as "mouse_proxy".
 */
class mouse_proxy : virtual public widget
{
public:
	using mouse_button_handler_type = std::function<event_status(mouse_proxy& w, const mouse_button_event&)>;
	using mouse_move_handler_type = std::function<event_status(mouse_proxy& w, const mouse_move_event&)>;
	using hovered_change_handler_type = std::function<void(mouse_proxy& w, unsigned pointer_id)>;

	struct parameters {
		mouse_button_handler_type mouse_button_handler;
		mouse_move_handler_type mouse_move_handler;
		hovered_change_handler_type hovered_change_handler;
	};

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		parameters mouse_proxy_params;
	};

	mouse_proxy(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	mouse_proxy(const mouse_proxy&) = delete;
	mouse_proxy& operator=(const mouse_proxy&) = delete;

	mouse_proxy(mouse_proxy&&) = delete;
	mouse_proxy& operator=(mouse_proxy&&) = delete;

	~mouse_proxy() override = default;

	/**
	 * @brief Mouse button event signal.
	 * Emitted when mouse button event reaches this widget.
	 */
	mouse_button_handler_type mouse_button_handler;

	event_status on_mouse_button(const mouse_button_event&) override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	mouse_move_handler_type mouse_move_handler;

	event_status on_mouse_move(const mouse_move_event& event) override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	hovered_change_handler_type hovered_change_handler;

	void on_hovered_change(unsigned pointer_id) override;
};

namespace make {
utki::shared_ref<ruis::mouse_proxy> mouse_proxy(
	utki::shared_ref<ruis::context> context, //
	ruis::mouse_proxy::all_parameters params
);
} // namespace make

} // namespace ruis
