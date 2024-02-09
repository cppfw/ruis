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

#include "image_push_button.hpp"

using namespace ruis;

void image_push_button::on_pressed_change()
{
	this->image_button::on_pressed_change();
	this->push_button::on_pressed_change();
}

image_push_button::image_push_button(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	button(this->context, desc),
	push_button(this->context, desc),
	image_button(this->context, desc)
{}
