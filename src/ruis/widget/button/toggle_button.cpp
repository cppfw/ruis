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

#include "toggle_button.hpp"

#include "../../util/util.hpp"

using namespace ruis;

toggle_button::toggle_button(const utki::shared_ref<ruis::context>& context) :
	widget(context, {}, {}),
	button(context, button::parameters{})
{}

event_status toggle_button::on_mouse_button(const mouse_button_event& e)
{
	if (e.button != mouse_button::left) {
		return event_status::propagate;
	}

	if (e.action == button_action::press) {
		if (!this->is_currently_pressed) {
			// NOTE: sometimes pos is outside of the widget, for example, when the mouse is captured by this
			//       widget and some other mouse button is clicked, and then the left one is clicked outside
			//       of the widget. See push_button::on_mouse_button() for more details.
			if (this->overlaps(e.pos)) {
				this->is_currently_pressed = true;
				this->pointer_id = e.pointer_id;
			}
		}
	} else {
		if (this->is_currently_pressed && this->pointer_id == e.pointer_id) {
			this->is_currently_pressed = false;
			// change the checked state on release, same as push button detects a clicked event
			if (this->is_hovered()) {
				this->toggle();
			}
		}
	}

	return event_status::consumed;
}

void toggle_button::on_hovered_change(unsigned pointer_id)
{
	if (!this->is_hovered(pointer_id)) {
		// the pointer which has pressed the button has moved off of it, so a following release
		// of this pointer should not change the checked state
		if (this->is_currently_pressed && this->pointer_id == pointer_id) {
			this->is_currently_pressed = false;
		}
	}
}
