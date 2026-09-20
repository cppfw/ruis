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

#include "rectangle_text_field.hpp"

#include <utki/debug.hpp>

using namespace ruis;

using namespace ruis::length_literals;

rectangle_text_field::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	ruis::string text
) :
	rectangle_text_field(
		context,
		params,
		// clang-format off
		ruis::make::text_input(
			context, //
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.params = std::move(params.params.text_input)
			},
			std::move(text)
		)
		// clang-format on
	)
{}

rectangle_text_field::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text_input> text_input
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// Initialize rectangle first so it adds the text_input as a child
	// clang-format off
	rectangle(
		context,
		{
			.params = [&](){
				if(auto& l = params.params.rectangle.padding.container.layout; !l){
					l = layout::pile;
				}

				for(auto& b : params.params.rectangle.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}

				for(auto& r : params.params.rectangle.specific.corner_radii){
					if(r.get().is_undefined()){
						r  = context.get().style().get_len_gap_small();
					}
				}

				if(auto& c = params.params.rectangle.specific.fill_color; c.get().is_undefined()){
					c = context.get().style().get_color_background();
				}
				if(auto& c = params.params.rectangle.specific.stroke_color; c.get().is_undefined()){
					c = context.get().style().get_color_primary();
				}
				if(auto& l = params.params.rectangle.specific.stroke_width; l.get().is_undefined()){
					l = context.get().style().get_len_border();
				}

				return std::move(params.params.rectangle);
			}()
		},
		{
			text_input
		}
	),
	// clang-format on
	text_field(
		context, //
		text_input.get()
	)
{}

utki::shared_ref<ruis::rectangle_text_field> ruis::make::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_text_field::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::rectangle_text_field>(
		context, //
		std::move(params),
		std::move(text)
	);
}