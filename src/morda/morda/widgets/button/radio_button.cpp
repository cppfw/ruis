/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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
#include "choice_group.hpp"

#include "../../context.hpp"

using namespace morda;

namespace{

const auto D_Layout = treeml::read(R"qwertyuiop(
		@image{
			image{morda_img_radiobutton_bg}
		}
		@image{
			image{morda_img_radiobutton_tick}
		}
	)qwertyuiop");

}

radio_button::radio_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		choice_button(this->context, desc),
		pile(this->context, D_Layout)
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->set_visible(this->is_pressed());
}

void radio_button::on_press_change(){
	this->choice_button::on_press_change();
	this->checkWidget->set_visible(this->is_pressed());
	this->clear_cache();
}
