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

#include "push_button.hpp"
#include "nine_patch_button.hpp"

namespace morda{

class nine_patch_push_button :
		virtual public push_button,
		public nine_patch_button
{
public:
	nine_patch_push_button(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
protected:
	void on_press_change()override;
	
	bool on_mouse_button(const mouse_button_event& e)override{
		return this->push_button::on_mouse_button(e);
	}

	void on_hover_change(unsigned pointerID)override{
		this->push_button::on_hover_change(pointerID);
	}
};

}
