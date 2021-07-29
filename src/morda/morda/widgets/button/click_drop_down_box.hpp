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

#pragma once

#include "drop_down_box.hpp"
#include "nine_patch_push_button.hpp"

namespace morda{

class click_drop_down_box :
		virtual public widget,
		private nine_patch_push_button,
		public drop_down_box
{
	std::weak_ptr<widget> current_drop_down_menu;

	unsigned num_mouse_buttons_pressed = 0;
public:
	click_drop_down_box(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	click_drop_down_box(const click_drop_down_box&) = delete;
	click_drop_down_box& operator=(const click_drop_down_box&) = delete;

private:
	// index of the hovered item in the drop down menu
	int hovered_index = -1;

	bool on_mouse_button(const mouse_button_event& e)override;
	bool on_mouse_move(const mouse_move_event& e)override;

	std::shared_ptr<widget> wrap_item(std::shared_ptr<widget>&& w, size_t index);

	void show_drop_down_menu();

	void handle_mouse_button_up(bool is_first_button_up_event);
};

}
