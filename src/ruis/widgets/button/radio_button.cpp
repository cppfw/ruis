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

#include "radio_button.hpp"

#include "../../context.hpp"

#include "choice_group.hpp"

using namespace ruis;

namespace {

const auto radio_button_layout = tml::read(R"qwertyuiop(
		layout{pile}

		@image{
			image{ruis_img_radiobutton_bg}
		}
		@image{
			image{ruis_img_radiobutton_tick}
		}
	)qwertyuiop");

} // namespace

radio_button::radio_button(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	button(this->context, desc),
	toggle_button(this->context, desc),
	choice_button(this->context, desc),
	container(this->context, radio_button_layout),
	check_widget(*this->children().rbegin())
{
	this->check_widget.get().set_visible(this->is_pressed());
}

void radio_button::on_pressed_change()
{
	this->choice_button::on_pressed_change();
	this->check_widget.get().set_visible(this->is_pressed());
	this->clear_cache();
}
