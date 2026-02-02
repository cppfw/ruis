#include "flickable.hpp"

using namespace ruis::touch;

bool flickable::on_mouse_button(const mouse_button_event& event)
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

			// std::cout << "withtin scroll threshold\n";

			this->flickable_on_mouse_button(event);
			return true;
		case state::not_scrolling:
			[[fallthrough]];
		case state::within_scroll_threshold:
			utki::assert(!event.is_down, SL);
			this->cur_state = state::idle;

			// std::cout << "idle\n";

			return this->flickable_on_mouse_button(event);
		case state::scrolling:
			utki::assert(!event.is_down, SL);
			this->cur_state = state::idle;

			// std::cout << "idle\n";

			return true;
	}
}

bool flickable::on_mouse_move(const mouse_move_event& event)
{
	// Single touch mode.
	if (this->cur_state != state::idle) {
		if (event.pointer_id != this->cur_pointer_id) {
			return this->flickable_on_mouse_move(event);
		}
	}

	switch (this->cur_state) {
		default:
			utki::assert(false, SL);
			[[fallthrough]];
		case state::idle:
			return this->flickable_on_mouse_move(event);
		case state::within_scroll_threshold:
			{
				bool consumed = this->flickable_on_mouse_move(event);
				if (consumed) {
					this->cur_state = state::not_scrolling;

					// std::cout << "mouse move: consumed, not scrolling\n";

					return true;
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
						this->flickable_on_mouse_button(mbe);
					}

					this->flickable_scroll_by(-delta);
				}
			}
			return true;
		case state::not_scrolling:
			// std::cout << "mouse move: not scrolling\n";
			return this->flickable_on_mouse_move(event);
		case state::scrolling:
			{
				auto delta = event.pos - this->prev_touch_point;

				// std::cout << "mouse move: scrolling, delta: " << delta << "\n";
				this->flickable_scroll_by(-delta);
				this->prev_touch_point = event.pos;
				return true;
			}
	}
}
