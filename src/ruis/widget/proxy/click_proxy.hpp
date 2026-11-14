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

#include "mouse_proxy.hpp"

namespace ruis {

/**
 * @brief A proxy widget which tracks pressed/click state by mouse.
 * Emits a pressed change handler and a click handler in response to mouse
 * button events.
 */
class click_proxy : virtual public mouse_proxy
{
	bool is_pressed_v = false;

public:
	/**
	 * @brief Mouse press state change handler type.
	 */
	using pressed_change_handler_type = std::function<void(click_proxy& w)>;

	/**
	 * @brief Click event handler type.
	 */
	using click_handler_type = std::function<void(click_proxy& w)>;

	/**
	 * @brief Click proxy specific parameters.
	 */
	struct parameters {
		/**
		 * @brief Mouse press state change handler.
		 */
		pressed_change_handler_type pressed_change_handler;

		/**
		 * @brief Click event handler.
		 */
		click_handler_type click_handler;
	};

	/**
	 * @brief All click proxy parameters.
	 */
	struct all_parameters {
		/**
		 * @brief Layout parameters.
		 */
		ruis::layout_parameters layout_params;

		/**
		 * @brief Base widget parameters.
		 */
		ruis::widget::parameters widget;

		/**
		 * @brief Click proxy specific parameters.
		 */
		parameters click_proxy_params;

		/**
		 * @brief Mouse proxy parameters.
		 */
		mouse_proxy::parameters mouse_proxy_params;
	};

	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - click proxy parameters.
	 */
	click_proxy(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	click_proxy(const click_proxy&) = delete;
	click_proxy& operator=(const click_proxy&) = delete;

	click_proxy(click_proxy&&) = delete;
	click_proxy& operator=(const click_proxy&&) = delete;

	/**
	 * @brief Destructor.
	 */
	~click_proxy() override = default;

	/**
	 * @brief Handle a mouse button event.
	 * @param event - the mouse button event.
	 * @return event processing status.
	 */
	event_status on_mouse_button(const mouse_button_event& event) override;

	/**
	 * @brief Invoked when the hovered state changes.
	 * @param pointer_id - id of the mouse pointer which changed the hovered state.
	 */
	void on_hovered_change(unsigned pointer_id) override;

	/**
	 * @brief Check if the click proxy is currently pressed.
	 * @return true if the click proxy is pressed.
	 */
	bool is_pressed() const noexcept
	{
		return this->is_pressed_v;
	}

	/**
	 * @brief Handler for mouse press state changes.
	 * The event is always consumed, because otherwise the mouse is not captured and we
	 * never receive the mbutton up event neede to detect a button click.
	 */
	pressed_change_handler_type pressed_change_handler;

	/**
	 * @brief Handler for clicked event.
	 */
	click_handler_type click_handler;
};

namespace make {
/**
 * @brief Construct a click proxy widget.
 * @param context - ruis context.
 * @param params - click proxy parameters.
 * @return newly constructed click proxy widget.
 */
utki::shared_ref<ruis::click_proxy> click_proxy(
	const utki::shared_ref<ruis::context>& context, //
	ruis::click_proxy::all_parameters params
);
} // namespace make

} // namespace ruis
