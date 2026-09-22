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
	// Build the label and the text field first so the labeled_text_field base can be
	// passed references to already-constructed widgets.
	labeled_rectangle_text_field(
		context, //
		params,
		ruis::make::text(
			context, //
			{
				.layout_params{
							   .dims = {ruis::dim::fill, ruis::dim::min},
							   .align = {ruis::align::front, ruis::align::center}
				},
				.params =
					[&]() {
						if (auto& c = params.params.label.text.color; c.get().is_undefined()) {
							c = context.get().style().get_color_text();
						}
						return std::move(params.params.label.text);
							   }
                ()
},
			std::move(params.params.label.string)
		),
		ruis::make::rectangle_text_field(
			context, //
			{.layout_params{.dims = {ruis::dim::max, ruis::dim::max}},
			 .params = std::move(params.params.rectangle_text_field)},
			std::move(text)
		)
	)
{}

labeled_rectangle_text_field::labeled_rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text> label,
	utki::shared_ref<ruis::rectangle_text_field> text_field
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// Initialize container first so it adds the label and the text field as its children
	// clang-format off
	container(
		context, //
		{
			.params{
				.layout = ruis::layout::column
			}
		},
		{
			label,
			text_field
		}
	),
	// clang-format on
	labeled_text_field(
		context, //
		text_field.get().get_text_input(),
		label.get()
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
