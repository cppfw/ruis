/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../base/color_widget.hpp"

namespace ruis {

/**
 * @brief Color widget.
 * This is a widget which can display a rectangle of a single color.
 * From GUI script it can be instantiated as "color".
 */
class color : public color_widget
{
public:
	color(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc);

	color(
		utki::shared_ref<ruis::context> context,
		widget::parameters widget_params,
		color_widget::parameters color_widget_params
	);

	color(const color&) = delete;
	color& operator=(const color&) = delete;

	color(color&&) = delete;
	color& operator=(color&&) = delete;

	~color() override = default;

	void render(const ruis::matrix4& matrix) const override;
};

} // namespace ruis
