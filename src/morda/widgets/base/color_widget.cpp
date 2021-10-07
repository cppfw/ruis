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

#include "color_widget.hpp"

#include "../../util/util.hpp"

using namespace morda;

color_widget::color_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "color"){
			this->color = get_property_value(p).to_uint32();
		}else if(p.value == "disabled_color"){
			this->disabled_color = get_property_value(p).to_uint32();
		}
	}
}

void color_widget::set_color(uint32_t color){
	if(this->color == color){
		return;
	}

	this->color = color;
	this->clear_cache();
}

void color_widget::set_disabled_color(uint32_t color){
	if(this->disabled_color == color){
		return;
	}

	this->disabled_color = color;
	this->clear_cache();
}

uint32_t color_widget::get_current_color()const noexcept{
	if(this->is_enabled()){
		return this->get_color();
	}else{
		return this->get_disabled_color();
	}
}
