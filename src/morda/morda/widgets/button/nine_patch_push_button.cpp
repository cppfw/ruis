/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "nine_patch_push_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

nine_patch_push_button::nine_patch_push_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		push_button(this->context, desc),
		nine_patch_button(this->context, desc)
{
	if(!this->get_unpressed_nine_patch()){
		this->set_unpressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_button_normal"));
	}
	
	if(!this->get_pressed_nine_patch()){
		this->set_pressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_button_pressed"));
	}
	
	this->on_press_change();
}

void nine_patch_push_button::on_press_change(){
	this->nine_patch_button::on_press_change();
	this->push_button::on_press_change();
}
