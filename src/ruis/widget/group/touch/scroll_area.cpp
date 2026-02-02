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
	return this->flickable::on_mouse_button(event);
}

bool scroll_area::on_mouse_move(const mouse_move_event& event)
{
	return this->flickable::on_mouse_move(event);
}

bool scroll_area::flickable_on_mouse_button(const mouse_button_event& event)
{
	return this->ruis::scroll_area::on_mouse_button(event);
}

bool scroll_area::flickable_on_mouse_move(const mouse_move_event& event)
{
	return this->ruis::scroll_area::on_mouse_move(event);
}

void scroll_area::flickable_scroll_by(const vec2& delta)
{
	this->scroll_by(delta);
}
