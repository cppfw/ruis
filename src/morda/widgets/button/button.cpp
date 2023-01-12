/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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


using namespace morda;

button::button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "pressed"){
			this->isPressed_v = get_property_value(p).to_bool();
		}
	}
}


void button::set_pressed(bool pressed){
	if(this->isPressed_v == pressed){
		return;
	}
	this->isPressed_v = pressed;
	this->isPressedChangedNotified = false;
	this->on_press_change();
}



void button::on_press_change(){
	if(this->isPressedChangedNotified){
		return;
	}
	this->isPressedChangedNotified = true;
	
	if(this->press_handler){
		this->press_handler(*this);
	}
}
