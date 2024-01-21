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

#include "../../widget.hpp"

namespace morda {

/**
 * @brief Basic widget which has a color attribute.
 *
 * @li @c color - color value.
 * @li @c disabled_color - color value for disabled state.
 */
class color_widget : public virtual widget
{
	constexpr static auto default_color = 0xffffffff;
	uint32_t color = default_color;

	constexpr static auto default_disabled_color = 0xff808080;
	uint32_t disabled_color = default_disabled_color;

protected:
	color_widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	color_widget(color_widget&&) = delete;
	color_widget& operator=(color_widget&&) = delete;

	~color_widget() override = default;

	void set_color(uint32_t color);

	uint32_t get_color() const noexcept
	{
		return this->color;
	}

	void set_disabled_color(uint32_t color);

	uint32_t get_disabled_color() const noexcept
	{
		return this->disabled_color;
	}

	/**
	 * @brief Get color for current enabled/disabled state.
	 * @return color for the current enabled/disabled state.
	 */
	uint32_t get_current_color() const noexcept;
};

} // namespace morda
