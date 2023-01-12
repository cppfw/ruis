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

#include "image_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

void image_button::update_image(){
	if(this->is_pressed()){
		this->set_image(this->pressedImage_v);
	}else{
		this->set_image(this->unpressedImage_v);
	}
}

void image_button::on_press_change(){
	this->update_image();
	this->button::on_press_change();
}

image_button::image_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		image(this->context, desc)
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

				if(pp.value == "pressed"){
					this->pressedImage_v = this->context->loader.load<res::image>(get_property_value(pp).to_string());
				}else if(pp.value == "unpressed"){
					this->unpressedImage_v = this->context->loader.load<res::image>(get_property_value(pp).to_string());
				}
				this->update_image();
			}
		}
	}
}

void image_button::set_pressed_image(std::shared_ptr<const res::image> image){
	this->pressedImage_v = std::move(image);
	this->update_image();
}

void image_button::set_unpressed_image(std::shared_ptr<const res::image> image){
	this->unpressedImage_v = std::move(image);
	this->update_image();
}
