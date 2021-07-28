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

#include "../widget.hpp"

namespace morda{

/**
 * @brief Basic widget which has a color setting.
 *
 * @param color - color value.
 * @param disabled_color - color value for disabled state.
 */
class color_widget : public virtual widget{
	uint32_t color = 0xffffffff;
	uint32_t disabled_color = 0xff808080;
protected:
	color_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	void set_color(uint32_t color);

	uint32_t get_color()const noexcept{
		return this->color;
	}

	void set_disabled_color(uint32_t color);

	uint32_t get_disabled_color()const noexcept{
		return this->disabled_color;
	}

	/**
	 * @brief Get color for current enabled/disabled state.
	 * @return color for the current enabled/disabled state.
	 */
	uint32_t get_current_color()const noexcept;
};

}
