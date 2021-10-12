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

#include "choice_button.hpp"

#include "choice_group.hpp"

using namespace morda;

choice_button::choice_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc)
{}

void choice_button::on_press_change(){
	this->toggle_button::on_press_change();

	if(this->is_pressed()){
		this->activate();
	}
}

bool choice_button::on_mouse_button(const mouse_button_event& e){
	if(this->is_pressed()){
		return true;
	}
	return this->toggle_button::on_mouse_button(e);
}

void choice_button::activate(){
	this->set_pressed(true);

	auto cg = this->try_get_ancestor<choice_group>();
	if(!cg){
		return;
	}
	
	cg->set_active_choice_button(utki::make_shared_from(*this));
}
