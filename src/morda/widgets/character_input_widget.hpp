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

#pragma once

#include "widget.hpp"

#include "../util/key.hpp"

namespace morda{

/**
 * @brief Base class for widgets which want to get character input events.
 * When the widget has keyboard focus it will also get character input events.
 */
class character_input_widget : virtual public widget{

protected:
	character_input_widget(const utki::shared_ref<morda::context>& c) :
			widget(c, treeml::forest())
	{}

public:
	/**
	 * @brief Handler for character input.
	 * @param e - character input event info.
	 */
	virtual void on_character_input(const character_input_event& e) = 0;

	virtual ~character_input_widget()noexcept{}
};


}
