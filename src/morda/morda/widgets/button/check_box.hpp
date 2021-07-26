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

#include "toggle_button.hpp"
#include "../label/nine_patch.hpp"

namespace morda{

/**
 * @brief Checkbox widget.
 * Checkbox is a toggle button which shows tick sign when checked and empty box when unchecked.
 * In GUI script can be instantiated as 'Checkbox'. Only available after initializing
 * standard morda widgets.
 */
class check_box :
		virtual public toggle_button,
		private nine_patch
{
	std::shared_ptr<widget> checkWidget;
public:
	check_box(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	check_box(const check_box&) = delete;
	check_box& operator=(const check_box&) = delete;

	bool on_mouse_button(const mouse_button_event& e)override{
		return this->toggle_button::on_mouse_button(e);
	}

	void on_hover_change(unsigned pointer_id)override{
		this->toggle_button::on_hover_change(pointer_id);
	}

	void on_press_change()override;

private:

};

}
