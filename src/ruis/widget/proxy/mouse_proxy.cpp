/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "mouse_proxy.hpp"

using namespace ruis;

mouse_proxy::mouse_proxy(utki::shared_ref<ruis::context> context, all_parameters params) :
	widget(std::move(context), widget::all_parameters{std::move(params.widget_params)})
{}

mouse_proxy::mouse_proxy(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc)
{}

bool mouse_proxy::on_mouse_button(const mouse_button_event& e)
{
	if (this->mouse_button_handler) {
		return this->mouse_button_handler(*this, e);
	}
	return false;
}

bool mouse_proxy::on_mouse_move(const mouse_move_event& e)
{
	// LOG("e.pos = " << e.pos << ", rect() = " << this->rect() << std::endl)
	if (this->mouse_move_handler) {
		return this->mouse_move_handler(*this, e);
	}
	return false;
}

void mouse_proxy::on_hovered_change(unsigned pointer_id)
{
	if (this->hovered_change_handler) {
		this->hovered_change_handler(*this, pointer_id);
	}
}
