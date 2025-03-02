/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "click_proxy.hpp"

using namespace ruis;

click_proxy::click_proxy( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	)
{}

bool click_proxy::on_mouse_button(const mouse_button_event& e)
{
	bool ret = false;

	if (e.button != mouse_button::left) {
		return false;
	}

	if (e.is_down) {
		this->is_pressed_v = true;
		if (this->pressed_change_handler) {
			ret = this->pressed_change_handler(*this);
		}
	} else {
		if (this->is_pressed()) {
			this->is_pressed_v = false;
			if (this->pressed_change_handler) {
				ret = this->pressed_change_handler(*this);
			}
			if (this->click_handler) {
				this->click_handler(*this);
			}
		} else {
			ret = this->deferred_release_ret;
		}
	}

	return ret;
}

void click_proxy::on_hovered_change(unsigned pointer_id)
{
	// TRACE(<< "on_hover_change(): hovered = " << this->is_hovered() << " pointer_id = " << pointer_id << std::endl)
	if (pointer_id != 0) {
		return;
	}

	if (!this->is_hovered(0)) {
		if (this->is_pressed()) {
			this->is_pressed_v = false;
			if (this->pressed_change_handler) {
				this->deferred_release_ret = this->pressed_change_handler(*this);
			}
		}
	}
}
