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

#include "list.hpp"

using namespace ruis::touch;

list::list(utki::shared_ref<ruis::context> context, all_parameters params) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	ruis::list(this->context, std::move(params)),
    flickable(this->context)
{}

ruis::event_status list::on_mouse_button(const mouse_button_event& event)
{
	return this->flickable::on_mouse_button(event);
}

ruis::event_status list::on_mouse_move(const mouse_move_event& event)
{
	return this->flickable::on_mouse_move(event);
}

ruis::event_status list::flickable_on_mouse_button(const mouse_button_event& event)
{
	return this->ruis::list::on_mouse_button(event);
}

ruis::event_status list::flickable_on_mouse_move(const mouse_move_event& event)
{
	return this->ruis::list::on_mouse_move(event);
}

ruis::vec2 list::flickable_scroll_by(const vec2& delta)
{
	if (this->is_vertical()) {
		return {0, this->scroll_by(delta.y())};
	} else {
		return {this->scroll_by(delta.x()), 0};
	}
}
