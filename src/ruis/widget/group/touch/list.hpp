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

#include "../../base/touch/flickable.hpp"
#include "../list.hpp"

namespace ruis::touch {

/**
 * @brief A touch-enabled scrollable list widget.
 * Combines a scrollable list with touch-based flickable scrolling.
 */
class list :
	public ruis::list, //
	private flickable
{
public:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - touch list widget parameters.
	 */
	list(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	/**
	 * @brief Handle a mouse button event.
	 * Routes the event to the flickable for gesture processing.
	 * @param event - the mouse button event.
	 * @return event processing status.
	 */
	event_status on_mouse_button(const mouse_button_event& event) override;

	/**
	 * @brief Handle a mouse move event.
	 * Routes the event to the flickable for gesture processing.
	 * @param event - the mouse move event.
	 * @return event processing status.
	 */
	event_status on_mouse_move(const mouse_move_event& event) override;

private:
	/**
	 * @brief Handle a mouse button event for flickable scrolling.
	 * @param event - the mouse button event.
	 * @return event processing status.
	 */
	event_status flickable_on_mouse_button(const mouse_button_event& event) override;

	/**
	 * @brief Handle a mouse move event for flickable scrolling.
	 * @param event - the mouse move event.
	 * @return event processing status.
	 */
	event_status flickable_on_mouse_move(const mouse_move_event& event) override;

	/**
	 * @brief Scroll the list by given number of pixels.
	 * @param delta - number of pixels to scroll by.
	 * @return number of pixels actually scrolled by.
	 */
	vec2 flickable_scroll_by(const vec2& delta) override;
};

namespace make {
/**
 * @brief Construct a touch list widget.
 * @param context - ruis context.
 * @param params - touch list widget parameters.
 * @return newly constructed touch list widget.
 */
utki::shared_ref<ruis::touch::list> list(
	const utki::shared_ref<ruis::context>& context, //
	ruis::touch::list::all_parameters params
);
} // namespace make

} // namespace ruis::touch
