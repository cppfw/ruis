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

#include "scroll_area.hpp"

using namespace ruis::touch;

scroll_area::scroll_area(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	ruis::scroll_area(
		this->context,
		{},
		std::move(children)
	)
// clang-format on
{}

bool scroll_area::on_mouse_button(const mouse_button_event& event)
{
	if (event.button != mouse_button::left) {
		return false;
	}

	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (this->cur_pointer_id != event.pointer_id) {
			return false;
		}
	}

	utki::assert(this->cur_state == state::idle || event.pointer_id == this->cur_pointer_id, SL);

	switch (this->cur_state) {
		default:
			utki::assert(false, SL);
			[[fallthrough]];
		case state::idle:
			utki::assert(event.is_down, SL);

			this->cur_state = state::within_scroll_threshold;
			this->prev_touch_point = event.pos;
			this->cur_pointer_id = event.pointer_id;

			std::cout << "withtin scroll threshold\n";

			ruis::scroll_area::on_mouse_button(event);
			return true;
		case state::not_scrolling:
			[[fallthrough]];
		case state::within_scroll_threshold:
			utki::assert(!event.is_down, SL);
			this->cur_state = state::idle;

			std::cout << "idle\n";

			return ruis::scroll_area::on_mouse_button(event);
		case state::scrolling:
			utki::assert(!event.is_down, SL);
			this->cur_state = state::idle;

			std::cout << "idle\n";

			return true;
	}
}

bool scroll_area::on_mouse_move(const mouse_move_event& event)
{
	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (event.pointer_id != this->cur_pointer_id) {
			return this->ruis::scroll_area::on_mouse_move(event);
		}
	}

	switch (this->cur_state) {
		default:
			utki::assert(false, SL);
			[[fallthrough]];
		case state::idle:
			return ruis::scroll_area::on_mouse_move(event);
		case state::within_scroll_threshold:
			{
				bool consumed = ruis::scroll_area::on_mouse_move(event);
				if (consumed) {
					this->cur_state = state::not_scrolling;

					std::cout << "mouse move: consumed, not scrolling\n";

					return true;
				}
				vec2 delta = event.pos - this->prev_touch_point;
				vec2 abs_delta = abs(delta);

				std::cout << "mouse move: within scroll threshold, delta: " << delta << ", abs_delta: " << abs_delta
						  << "\n";

				if (abs_delta.x() > this->scroll_threshold_px || abs_delta.y() > this->scroll_threshold_px) {
					this->cur_state = state::scrolling;

					std::cout << "scrolling\n";

					this->prev_touch_point = event.pos;

					// send mouse button up event out of widget area to cancel any ongoing interactions
					{
						ruis::mouse_button_event mbe{
							false, // is_down
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
						this->ruis::scroll_area::on_mouse_button(mbe);
					}

					this->scroll_by(-delta);
				}
			}
			return true;
		case state::not_scrolling:
			std::cout << "mouse move: not scrolling\n";
			return ruis::scroll_area::on_mouse_move(event);
		case state::scrolling:
			{
				auto delta = event.pos - this->prev_touch_point;

				std::cout << "mouse move: scrolling, delta: " << delta << "\n";
				this->scroll_by(-delta);
				this->prev_touch_point = event.pos;
				return true;
			}
	}
}
