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

#include "nine_patch_button.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

nine_patch_button::nine_patch_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		nine_patch(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(pp.value == "unpressed"){
					this->set_unpressed_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(pp).to_string()));
				}else if(pp.value == "pressed"){
					this->set_pressed_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(pp).to_string()));
				}
			}
		}
	}
	this->on_press_change();
}

void nine_patch_button::on_press_change(){
	this->set_nine_patch(this->is_pressed() ? this->pressedNinePatch_v : this->unpressedNinePatch_v);
	this->button::on_press_change();
}

void nine_patch_button::set_pressed_nine_patch(std::shared_ptr<const res::nine_patch> np){
	this->pressedNinePatch_v = std::move(np);
}

void nine_patch_button::set_unpressed_nine_patch(std::shared_ptr<const res::nine_patch> np){
	this->unpressedNinePatch_v = std::move(np);
}
