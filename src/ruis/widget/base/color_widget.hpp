/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../../util/color.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Basic widget which has a color attribute.
 *
 * @li @c color - color value.
 * @li @c disabled_color - color value for disabled state.
 */
class color_widget : public virtual widget
{
public:
	struct parameters {
		constexpr static auto default_color = 0xffffffff;
		styled<ruis::color> color = default_color;

		constexpr static auto default_disabled_color = 0xff808080;
		styled<ruis::color> disabled_color = default_disabled_color;
	};

private:
	parameters params;

protected:
	color_widget(utki::shared_ref<ruis::context> context, parameters params);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	color_widget(color_widget&&) = delete;
	color_widget& operator=(color_widget&&) = delete;

	~color_widget() override = default;

	void set_color(styled<ruis::color> color);

	const color& get_color() const noexcept
	{
		return this->params.color.get();
	}

	void set_disabled_color(styled<ruis::color> color);

	const color& get_disabled_color() const noexcept
	{
		return this->params.disabled_color.get();
	}

	/**
	 * @brief Get color for current enabled/disabled state.
	 * @return color for the current enabled/disabled state.
	 */
	color get_current_color() const noexcept;

	/**
	 * @brief Color change callback.
	 * Invoked when color or disabled color changes.
	 */
	virtual void on_color_change() {}
};

} // namespace ruis
