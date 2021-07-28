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

#include "../label/nine_patch.hpp"
#include "choice_button.hpp"
#include "nine_patch_toggle.hpp"

namespace morda{

class tab :
		virtual public choice_button,
		public nine_patch_toggle
{
	bool maskOverlaps(vector2 pos);
	
public:
	tab(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	tab(const tab&) = delete;
	tab& operator=(const tab&) = delete;
	
protected:	
	bool on_mouse_button(const mouse_button_event& event)override;

	void on_hover_change(unsigned pointer_id)override{}
	
	void on_press_change()override;
};

}
