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
#include <utki/views.hpp>

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
			{
				utki::assert(event.action == button_action::press, SL);

				this->push_touch_move_to_history({
					.position = event.pos,
					.timestamp_ms = utki::get_ticks_ms()
				});
				auto vel = this->calculate_touch_velocity();
				std::cout << "touch press, vel = " << vel << std::endl;

				this->cur_state = state::within_scroll_threshold;
				this->prev_touch_point = event.pos;
				this->cur_pointer_id = event.pointer_id;

				// std::cout << "withtin scroll threshold\n";

				this->flickable_on_mouse_button(event);
				return event_status::consumed;
			}
		case state::not_scrolling:
			[[fallthrough]];
		case state::within_scroll_threshold:
			{
				utki::assert(event.action == button_action::release, SL);
				this->cur_state = state::idle;

				auto vel = this->calculate_touch_velocity();
				std::cout << "touch release, vel = " << vel << std::endl;

				this->touch_history.clear();

				return this->flickable_on_mouse_button(event);
			}
		case state::scrolling:
			{
				utki::assert(event.action == button_action::release, SL);
				this->cur_state = state::idle;

				// TODO: start updating

				auto vel = this->calculate_touch_velocity();
				std::cout << "touch release, vel = " << vel << std::endl;

				this->touch_history.clear();

				return event_status::consumed;
			}
	}
}

ruis::event_status flickable::on_mouse_move(const mouse_move_event& event)
{
	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (event.pointer_id != this->cur_pointer_id) {
			return this->flickable_on_mouse_move(event);
		}
	}else{
		// no touch active, ignore mouse move events
		return ruis::event_status::propagate;
	}

	this->push_touch_move_to_history({
		.position = event.pos,
		.timestamp_ms = utki::get_ticks_ms()
	});

	auto vel = this->calculate_touch_velocity();
	std::cout << "touch move, vel = " << vel << std::endl;

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

namespace{
constexpr auto max_history_records = 10;
}

void flickable::push_touch_move_to_history(touch_move_info tm){
	constexpr auto max_history_age_ms = 200;
	constexpr auto min_time_between_points_ms = 1;

	// drop too old records
	while(!this->touch_history.empty() && tm.timestamp_ms - this->touch_history.front().timestamp_ms > max_history_age_ms){
		this->touch_history.pop_front();
	}

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

ruis::vec2 flickable::calculate_touch_velocity(){
	if(this->touch_history.size() < 2){
		// std::cout << "flickable::calculate_touch_velocity(): return 0. this->touch_history.size() = " << this->touch_history.size() << std::endl;
		return {0};
	}

	if(this->touch_history.size() == 2){
		const auto& p1 = this->touch_history.front();
		const auto& p2 = this->touch_history.back();

		auto dt_ms = p2.timestamp_ms - p1.timestamp_ms;
		utki::assert(dt_ms > 0, SL);

		auto dp = p2.position - p1.position;

		auto vel = dp / ruis::real(dt_ms);

		// std::cout << "flickable::calculate_touch_velocity(): return " << vel << ". this->touch_history.size() = " << this->touch_history.size() << std::endl;
		return vel;
	}

	// use Ordinary Least Squares method to fit a quadratic curve to the points of touch history
	ruis::vec2 vel = calculate_touch_velocity_for_at_least_3_points_using_ols_method();

	// std::cout << "flickable::calculate_touch_velocity(): return " << vel << ". this->touch_history.size() = " << this->touch_history.size() << std::endl;
	return vel;
}

ruis::vec2 flickable::calculate_touch_velocity_for_at_least_3_points_using_ols_method(){
	// Ordinary Least Squares method fits quadratic curve y(t)=a*t^2+b*t+c to a set of n >= 3 points.
	// Coefficients a, b and c can be found by solving the following system of linear equations
	//
	// | sum(t^4) sum(t^3) sum(t^2) |   | a |   | sum(y * t^2) |
	// | sum(t^3) sum(t^2) sum(t)   | * | b | = | sum(y * t)   |
	// | sum(t^2) sum(t)   n        |   | c |   | sum(y)       |
	//
	// The touch velocity is effectively v = dy/dt = 2*a*t + b
	//
	// If we bias t foe each point to the last point, i.e. last point will be at t=0 and previous
	// points will be at some negative times, then v(0) = b.
	// Effecively the touch velocity is the b coefficient.

	// need at least 3 points for quadratic curve fitting
	utki::assert(this->touch_history.size() >= 3, SL);

	uint32_t time_bias_ms = this->touch_history.back().timestamp_ms;

	ruis::real time_sum = 0;
	ruis::real time_pow2_sum = 0;
	ruis::real time_pow3_sum = 0;
	ruis::real time_pow4_sum = 0;
	ruis::vec2 pos_time_pow2_sum = 0;
	ruis::vec2 pos_time_sum = 0;
	ruis::vec2 pos_sum = 0;

	for(const auto& rec : this->touch_history){
		// Here we still do arithmetics in uint32_t space.
		// The time_bias_ms is the latest timestamp, so subtract from it to get positive biased time.
		uint32_t shifted_time = time_bias_ms - rec.timestamp_ms;

		// since we bias to lates timestamp, the actual biased time should be negative
		ruis::real t = -ruis::real(shifted_time);

		ruis::real t_pow2 = t * t;
		ruis::real t_pow3 = t_pow2 * t;
		ruis::real t_pow4 = t_pow3 * t;

		// std::cout << "shifted time = " << t << std::endl;

		time_sum += t;
		time_pow2_sum += t_pow2;
		time_pow3_sum += t_pow3;
		time_pow4_sum += t_pow4;

		pos_sum += rec.position;
		pos_time_sum += rec.position * t;
		pos_time_pow2_sum += rec.position * t_pow2;
	}

	// clang-format off
	ruis::mat3 m{
		{time_pow4_sum, time_pow3_sum, time_pow2_sum},
		{time_pow3_sum, time_pow2_sum, time_sum},
		{time_pow2_sum, time_sum, this->touch_history.size()}
	};
	// clang-format on

	auto det = m.det();

	constexpr auto epsilon = ruis::real(1e-9);

	using std::abs;
	if(abs(det) < epsilon){
		// the matrix is not invertible
		return {0};
	}

	// replace 2nd column with right-hand side vector and calculate determinant
	ruis::vec2 det_b;
	for(auto [p0, p1, p2, out] : utki::views::zip(pos_time_pow2_sum, pos_time_sum, pos_sum, det_b)){
		m[0][1] = p0;
		m[1][1] = p1;
		m[2][1] = p2;
		out = m.det();
	}

	return det_b.comp_div(det);
}
