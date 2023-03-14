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

#include <functional>

#include "../../widget.hpp"

namespace morda{

/**
 * @brief Basic button widget class.
 * Button has two states: normal and pressed.
 */
class button : public virtual widget{
	bool isPressed_v = false;
	bool isPressedChangedNotified = true;

protected:
	/**
	 * @brief Called when button pressed state changes.
	 * Default implementation emits the 'pressedChanged' signal.
	 */
	virtual void on_press_change();
public:
	button(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	/**
	 * @brief Check if button is currently pressed.
	 * @return true if button is pressed.
	 * @return false otherwise.
	 */
	bool is_pressed()const noexcept{
		return this->isPressed_v;
	}

	void set_pressed(bool pressed);

	/**
	 * @brief Pressed changed signal.
	 * Emitted when button pressed state changes.
	 */
	std::function<void(button&)> press_handler;
};

}
