/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "rectangle_text_input_line.hpp"

#include <utki/debug.hpp>

using namespace ruis;

rectangle_text_input_line::rectangle_text_input_line(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	ruis::string text
) :
	rectangle_text_input_line(
		context,
		params,
		ruis::make::text_input_line(
			context, //
			{
				.layout_params = std::move(params.text_input_line_params.layout_params),
				.widget_params = std::move(params.text_input_line_params.widget_params),
				.text_widget_params = std::move(params.text_input_line_params.text_widget_params),
				.color_params = std::move(params.text_input_line_params.color_params)
			},
			std::move(text)
		)
	)
{}

rectangle_text_input_line::rectangle_text_input_line(
	utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text_input_line> text_input_line
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// Initialize rectangle first so it adds the text_input_line as a child
	// clang-format off
	rectangle(
		this->context,
		{
			.container_params = std::move(params.container_params),
			.padding_params = std::move(params.padding_params),
			.color_params = std::move(params.color_params),
			.rectangle_params = {
				.corner_radii = std::move(params.rectangle_params.corner_radii),
				.stroke_width = std::move(params.rectangle_params.stroke_width),
				// stroke_color = primary color from style
				.stroke_color = this->context.get().style().get_color_primary(),
				// fill color will be set from color_widget's color (background from style)
			}
		},
		{
			text_input_line
		}
	),
	// clang-format on
	decorated_widget<ruis::text_input_line>(
		this->context, //
		text_input_line.get()
	)
{
	// Set fill color to background color from style
	this->set_color(this->context.get().style().get_color_background());
}

void rectangle_text_input_line::on_focus_change()
{
	this->decorated_widget<ruis::text_input_line>::get_decorated().on_focus_change();
}

utki::shared_ref<ruis::rectangle_text_input_line> ruis::make::rectangle_text_input_line(
	utki::shared_ref<ruis::context> context, //
	ruis::rectangle_text_input_line::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::rectangle_text_input_line>(
		std::move(context), //
		std::move(params),
		std::move(text)
	);
}