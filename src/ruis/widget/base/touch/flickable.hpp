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

#include <deque>

#include "../../widget.hpp"

namespace ruis::touch {

/**
 * @brief A base class for widgets which support touch/mouse gestures such as
 * flick scrolling (pinching is planned for the future).
 * Handles the gesture recognition and the subsequent inertial scrolling.
 */
class flickable :
	virtual public ruis::widget, //
	private updateable
{
	constexpr static auto scroll_threshold_pp = 5;

	// TODO: convert from scroll_threshold_pp if dots per pp changes
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "constant value")
	real scroll_threshold_px = 10;

	/**
	 * @brief Flickable scrolling state.
	 */
	enum class state {
		/**
		 * @brief The user is not touching the flickable and no scrolling is happening.
		 */
		idle,

		/**
		 * @brief The user is touching the flickable, but has not yet moved the touch point enough to start scrolling.
		 * The flickable will not scroll until the touch point has moved enough to exceed the scroll threshold.
		 * This state is used to forward the mouse move events to the child widget that is being touched, until the scroll threshold is exceeded.
		 * Once the scroll threshold is exceeded, the flickable will start scrolling and will not forward the mouse move events to the child widget anymore.
		 */
		within_scroll_threshold,

		/**
		 * @brief The user is dragging the content.
		 * Some child widget of the flickable has consumed the mouse event and dragging is happening within that child widget.
		 * In that case the flickable will not scroll.
		 * This state is used to forward the mouse move events to the child widget that is being dragged.
		 */
		not_scrolling,

		/**
		 * @brief The user is dragging the content.
		 * The user is touching the flickable and the touch point is moving,
		 * so the flickable is scrolling following the touch point.
		 */
		dragging,

		/**
		 * @brief The flickable is scrolling inertially after the user has released the touch point.
		 */
		inertial_scrolling
	} cur_state = state::idle;

	vec2 prev_touch_point;

	// So far, the flickable only works in single touch mode.
	// This variable holds the pointer ID of the current touch.
	unsigned cur_pointer_id = std::numeric_limits<unsigned>::max();

	/**
	 * @brief A single touch move sample.
	 */
	struct touch_move_info {
		/**
		 * @brief Touch point position.
		 */
		vec2 position;

		/**
		 * @brief Timestamp of the touch point in milliseconds.
		 */
		uint32_t timestamp_ms;
	};

	std::deque<touch_move_info> touch_history;

	/**
	 * @brief Push a touch move sample to the touch history.
	 * @param tm - touch move sample to push.
	 */
	void push_touch_move_to_history(touch_move_info tm);

	/**
	 * @brief Calculate touch velocity from the touch history.
	 * @return touch velocity in pixels per millisecond.
	 */
	vec2 calculate_touch_velocity_px_per_ms();

	/**
	 * @brief Calculate touch velocity using the ordinary least squares method
	 * from at least 3 points of the touch history.
	 * @return touch velocity in pixels per millisecond.
	 */
	ruis::vec2 calculate_touch_velocity_for_at_least_3_points_using_ols_method_px_per_ms();

	// inertial scrolling state
	ruis::real friction; // TODONEXT: update if dots per pp changes
	ruis::vec2 velocity_px_per_ms;

public:
	/**
	 * @brief Handle a mouse button event.
	 * @param event - the mouse button event.
	 * @return event processing status.
	 */
	event_status on_mouse_button(const mouse_button_event& event) override;

	/**
	 * @brief Handle a mouse move event.
	 * @param event - the mouse move event.
	 * @return event processing status.
	 */
	event_status on_mouse_move(const mouse_move_event& event) override;

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 */
	flickable(const utki::shared_ref<ruis::context>& context);

	/**
	 * @brief Handle a mouse button event for flickable scrolling.
	 * The derived implementation should convey the event to the widget for regular processing.
	 * @param event - the mouse button event.
	 * @return event processing status.
	 */
	virtual event_status flickable_on_mouse_button(const mouse_button_event& event) = 0;

	/**
	 * @brief Handle a mouse move event for flickable scrolling.
	 * The derived implementation should convey the event to the widget for regular processing.
	 * @param event - the mouse move event.
	 * @return event processing status.
	 */
	virtual event_status flickable_on_mouse_move(const mouse_move_event& event) = 0;

	/**
	 * @brief Scroll the widget by the given number of pixels.
	 * The derived widget implements this to scroll the widget by the given delta.
	 * @param delta - number of pixels to scroll by.
	 * @return Number of pixels actually scrolled.
	 */
	virtual vec2 flickable_scroll_by(const vec2& delta) = 0;

private:
	/**
	 * @brief Update inertial scrolling state.
	 * @param dt_ms - number of milliseconds to update by.
	 */
	void update(uint32_t dt_ms) override;
};

} // namespace ruis::touch
