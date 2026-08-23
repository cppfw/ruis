/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "ellipse_button.hpp"

using namespace ruis;

void ellipse_button::render(const mat4& matrix) const
{
	this->ellipse::render(matrix);
}

void ellipse_button::on_pressed_change()
{
	this->update_color();
	this->button::on_pressed_change();
}

void ellipse_button::update_color()
{
	if (this->is_pressed()) {
		this->set_color(this->params.pressed_color);
	} else {
		this->set_color(this->params.unpressed_color);
	}
}

ellipse_button::ellipse_button( //
	utki::shared_ref<ruis::context> context,
	container::parameters container_params,
	padding::parameters padding_params,
	parameters params,
	widget_list contents //
) :
	widget(context, {}, {}),
	button(this->context, button::parameters{}),
	ellipse(
		this->context,
		// clang-format off
		ellipse::all_parameters{
			.container_params = std::move(container_params), 
            .padding_params = std::move(padding_params)
		},
		// clang-format on
		std::move(contents)
	),
	params([&]() {
		if (params.unpressed_color.get().is_undefined()) {
			params.unpressed_color = context.get().style().get_color_primary();
		}
		if (params.pressed_color.get().is_undefined()) {
			params.pressed_color = context.get().style().get_color_secondary();
		}

		return std::move(params);
	}())
{
	this->update_color();
}