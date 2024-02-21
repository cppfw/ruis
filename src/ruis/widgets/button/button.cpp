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

#include "button.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

button::button( //
	utki::shared_ref<ruis::context> context,
	parameters params
) :
	widget(std::move(context), widget::parameters{}),
	params(std::move(params))
{}

button::button(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "pressed") {
			this->params.pressed = get_property_value(p).to_bool();
		}
	}
}

void button::set_pressed(bool pressed)
{
	if (this->params.pressed == pressed) {
		return;
	}
	this->params.pressed = pressed;
	this->is_pressed_changed_notified = false;
	this->on_pressed_change();
}

void button::on_pressed_change()
{
	if (this->is_pressed_changed_notified) {
		return;
	}
	this->is_pressed_changed_notified = true;

	if (this->pressed_change_handler) {
		this->pressed_change_handler(*this);
	}
}
