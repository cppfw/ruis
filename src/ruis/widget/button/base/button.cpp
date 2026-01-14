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

#include "button.hpp"

#include "../../../context.hpp"
#include "../../../util/util.hpp"

using namespace ruis;

button::button(
	utki::shared_ref<ruis::context> context, //
	parameters params
) :
	widget(std::move(context), {}, {}),
	params(std::move(params))
{}

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
	// The button::on_pressed_change() can be called several times in a row because
	// button multiple inheritance hierarchy can be quite complex, and each derived class can call
	// button::on_pressed_change() from its own on_pressed_change() overridden method.
	// To avoid invoking this->pressed_change_handler many times without reason we use the
	// this->is_pressed_changed_notified flag.
	if (this->is_pressed_changed_notified) {
		return;
	}
	this->is_pressed_changed_notified = true;

	if (this->pressed_change_handler) {
		this->pressed_change_handler(*this);
	}
}
