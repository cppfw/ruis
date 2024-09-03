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

#include "toggle_button.hpp"

#include "../../util/util.hpp"

using namespace ruis;

toggle_button::toggle_button(utki::shared_ref<ruis::context> context) :
	widget(std::move(context), {}, {}),
	button(this->context, button::parameters{})
{}

toggle_button::toggle_button(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	button(this->context, desc)
{}

bool toggle_button::on_mouse_button(const mouse_button_event& e)
{
	if (e.button == mouse_button::left) {
		if (e.is_down) {
			this->toggle();
		}
		return true;
	}

	return false;
}
