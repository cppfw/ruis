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

#include "push_button.hpp"
#include "../label/nine_patch.hpp"

namespace morda{

/**
 * @brief Simple push button.
 * Implementation of a push button as a nine patch which can have children.
 * In GUI scripts can be instantiated as "SimpleButton". Only available after
 * initializing standard morda widgets.
 * Since the button is a nine_patch it can have children.
 */
class nine_patch_button :
		virtual public button,
		public nine_patch
{
	std::shared_ptr<const res::nine_patch> unpressedNinePatch_v;
	std::shared_ptr<const res::nine_patch> pressedNinePatch_v;
	
public:
	nine_patch_button(const nine_patch_button&) = delete;
	nine_patch_button& operator=(const nine_patch_button&) = delete;
	
	void set_unpressed_nine_patch(std::shared_ptr<const res::nine_patch> np);
	
	const decltype(unpressedNinePatch_v)& get_unpressed_nine_patch()const noexcept{
		return this->unpressedNinePatch_v;
	}
	
	void set_pressed_nine_patch(std::shared_ptr<const res::nine_patch> np);
	
	const decltype(pressedNinePatch_v)& get_pressed_nine_patch()const noexcept{
		return this->pressedNinePatch_v;
	}
	
protected:
	nine_patch_button(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	void on_press_change()override;
};

}
