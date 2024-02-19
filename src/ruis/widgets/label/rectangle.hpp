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
 * @brief Rectangle widget.
 * This is a widget which can display a rectangle of a single color.
 */
class rectangle : public color_widget
{
public:
	rectangle(
		utki::shared_ref<ruis::context> context,
		widget::parameters widget_params,
		color_widget::parameters color_widget_params
	);

	rectangle(const rectangle&) = delete;
	rectangle& operator=(const rectangle&) = delete;

	rectangle(rectangle&&) = delete;
	rectangle& operator=(rectangle&&) = delete;

	~rectangle() override = default;

	void render(const ruis::matrix4& matrix) const override;
};

namespace make {

[[deprecated]]
inline utki::shared_ref<ruis::widget> rectangle(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::color_widget::parameters color_widget_params
)
{
	return utki::make_shared<ruis::rectangle>(
		std::move(context),
		std::move(widget_params),
		std::move(color_widget_params)
	);
}

struct rectangle_parameters {
	widget::parameters widget_params;
	color_widget::parameters color_params;
};

inline utki::shared_ref<ruis::widget> rectangle(utki::shared_ref<ruis::context> context, rectangle_parameters params)
{
	return utki::make_shared<ruis::rectangle>(
		std::move(context),
		std::move(params.widget_params),
		std::move(params.color_params)
	);
}

} // namespace make

} // namespace ruis
