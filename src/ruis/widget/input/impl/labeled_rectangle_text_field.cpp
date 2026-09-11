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

#include "labeled_rectangle_text_field.hpp"

#include <utki/debug.hpp>

#include "../../label/text.hpp"

using namespace ruis;

labeled_rectangle_text_field::labeled_rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	ruis::string text
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// Initialize container first so it adds the label and the text field as its children
	// clang-format off
	container(
		context, //
		{
			.container_params{
				.layout = ruis::layout::column
			}
		},
		{
			ruis::make::text(
				context, //
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::min},
						.align = {ruis::align::front, ruis::align::center}
					},
					.params = [&](){
						if(params.label_params.text_params.color_params.color.get().is_undefined()){
							params.label_params.text_params.color_params.color = context.get().style().get_color_text_secondary();
						}
						return std::move(params.label_params.text_params);
					}()
				},
				std::move(params.label_params.text)
			),
			ruis::make::rectangle_text_field(
				context, //
				{
					.layout_params{
						.dims = {ruis::dim::max, ruis::dim::max}
					},
					.params = std::move(params.rectangle_text_field_params)
				},
				std::move(text)
			)
		}
	),
	// clang-format on
	labeled_text_field(
		context, //
		this->get_text_input(),
		this->get_label()
	)
{}

utki::shared_ref<ruis::labeled_rectangle_text_field> ruis::make::labeled_rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::labeled_rectangle_text_field::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::labeled_rectangle_text_field>(
		context, //
		std::move(params),
		std::move(text)
	);
}
