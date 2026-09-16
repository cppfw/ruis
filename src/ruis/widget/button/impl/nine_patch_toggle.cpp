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

#include "nine_patch_toggle.hpp"

using namespace ruis;

void nine_patch_toggle::on_pressed_change()
{
	this->nine_patch_button::on_pressed_change();
	this->toggle_button::on_pressed_change();
}

nine_patch_toggle::nine_patch_toggle(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	widget_list children
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	button(context, {}),
	toggle_button(context),
	nine_patch_button(
		context, //
		std::move(params.params.nine_patch_button),
		std::move(children)
	)
{}

event_status nine_patch_toggle::on_mouse_button(const mouse_button_event& e)
{
	if (this->nine_patch_button::on_mouse_button(e) == event_status::consumed) {
		return event_status::consumed;
	}
	return this->toggle_button::on_mouse_button(e);
}
