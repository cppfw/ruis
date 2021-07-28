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

#pragma once

#include "button.hpp"


namespace morda{

/**
 * @brief Basic class of a toggle button.
 * Toggle button is a button which can be checked and unchecked.
 * In GUI script it has the 'checked{true/false}' attribute which can set initial
 * checked state of the widget.
 */
class toggle_button : virtual public button{
protected:
	bool on_mouse_button(const mouse_button_event& event)override;
	
public:
	toggle_button(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	/**
	 * @brief Change checked state to opposite.
	 */
	void toggle(){
		this->set_pressed(!this->is_pressed());
	}
};

}
