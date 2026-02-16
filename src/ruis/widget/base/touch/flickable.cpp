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

#include "flickable.hpp"

#include <utki/time.hpp>

using namespace ruis::touch;

ruis::event_status flickable::on_mouse_button(const mouse_button_event& event)
{
	if (event.button != mouse_button::left) {
		return event_status::propagate;
	}

	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (this->cur_pointer_id != event.pointer_id) {
			return event_status::propagate;
		}
	}

	utki::assert(this->cur_state == state::idle || event.pointer_id == this->cur_pointer_id, SL);

	switch (this->cur_state) {
		default:
			utki::assert(false, SL);
			[[fallthrough]];
		case state::idle:
			utki::assert(event.action == button_action::press, SL);

			this->push_touch_move_to_history({
				.position = event.pos,
				.timestamp_ms = utki::get_ticks_ms()
			});

			this->cur_state = state::within_scroll_threshold;
			this->prev_touch_point = event.pos;
			this->cur_pointer_id = event.pointer_id;

			// std::cout << "withtin scroll threshold\n";

			this->flickable_on_mouse_button(event);
			return event_status::consumed;
		case state::not_scrolling:
			[[fallthrough]];
		case state::within_scroll_threshold:
			utki::assert(event.action == button_action::release, SL);
			this->cur_state = state::idle;

			// std::cout << "idle\n";

			return this->flickable_on_mouse_button(event);
		case state::scrolling:
			utki::assert(event.action == button_action::release, SL);
			this->cur_state = state::idle;

			// TODO: start updating

			// std::cout << "idle\n";

			return event_status::consumed;
	}
}

ruis::event_status flickable::on_mouse_move(const mouse_move_event& event)
{
	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (event.pointer_id != this->cur_pointer_id) {
			return this->flickable_on_mouse_move(event);
		}
	}

	this->push_touch_move_to_history({
		.position = event.pos,
		.timestamp_ms = utki::get_ticks_ms()
	});

	switch (this->cur_state) {
		default:
			[[fallthrough]];
		case state::idle:
			utki::assert(false, SL);
			return this->flickable_on_mouse_move(event);
		case state::within_scroll_threshold:
			{
				if (this->flickable_on_mouse_move(event) == event_status::consumed) {
					this->cur_state = state::not_scrolling;

					// std::cout << "mouse move: consumed, not scrolling\n";

					return event_status::consumed;
				}

				vec2 delta = event.pos - this->prev_touch_point;
				vec2 abs_delta = abs(delta);

				// std::cout << "mouse move: within scroll threshold, delta: " << delta << ", abs_delta: " << abs_delta << "\n";

				if (abs_delta.x() > this->scroll_threshold_px || abs_delta.y() > this->scroll_threshold_px) {
					this->cur_state = state::scrolling;

					// std::cout << "scrolling\n";

					this->prev_touch_point = event.pos;

					// send mouse button up event out of widget area to cancel any ongoing interactions
					{
						ruis::mouse_button_event mbe{
							button_action::release,
							[]() {
								using std::numeric_limits;

								if constexpr (numeric_limits<ruis::real>::has_infinity) {
									return -numeric_limits<ruis::real>::infinity();
								} else {
									return numeric_limits<ruis::real>::min();
								}
							}(),
							mouse_button::left,
							this->cur_pointer_id
						};
						this->flickable_on_mouse_button(mbe);
					}

					this->flickable_scroll_by(-delta);
				}
			}
			return event_status::consumed;
		case state::not_scrolling:
			// std::cout << "mouse move: not scrolling\n";
			return this->flickable_on_mouse_move(event);
		case state::scrolling:
			{
				vec2 delta = event.pos - this->prev_touch_point;
				// std::cout << "mouse move: scrolling, delta: " << delta << "\n";
				this->flickable_scroll_by(-delta);
				this->prev_touch_point = event.pos;
				return event_status::consumed;
			}
	}
}

void flickable::update(uint32_t dt_ms)
{
	// TODO:
}

void flickable::push_touch_move_to_history(touch_move_info tm){
	constexpr auto max_history_records = 10;
	constexpr auto max_history_age_ms = 200;
	constexpr auto min_time_between_points_ms = 1;

	if(this->touch_history.empty()){
		this->touch_history.push_back(std::move(tm));
		return;
	}

	utki::assert(!this->touch_history.empty(), SL);
	auto& last_record = this->touch_history.back();

	if(tm.timestamp_ms - last_record.timestamp_ms <= min_time_between_points_ms){
		// we are too close in time to the last record, just update the last record
		last_record = tm;
		return;
	}

	if(this->touch_history.size() == max_history_records){
		this->touch_history.pop_front();
	}

	this->touch_history.push_back(std::move(tm));
};
