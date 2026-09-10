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

#include "nine_patch_text_field.hpp"

#include <utki/debug.hpp>

using namespace std::string_view_literals;

using namespace ruis;

nine_patch_text_field::nine_patch_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	ruis::string text
) :
	nine_patch_text_field(
		context,
		params,
		// clang-format off
		ruis::make::text_input(
			context, //
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.color_params = [&](){
					if(params.color_params.color.get().is_undefined()){
						params.color_params.color = context.get().style().get_color_text();
					}

					return std::move(params.color_params);
				}(),
				.text_widget_params = std::move(params.text_widget_params),
				.text_input_params = [&](){
					if(params.text_field_params.hint_color.get().is_undefined()){
						params.text_field_params.hint_color = context.get().style().get_color_text_secondary();
					}
					return std::move(params.text_field_params);
				}()
			},
			std::move(text)
		)
		// clang-format on
	)
{}

nine_patch_text_field::nine_patch_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text_input> text_input
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// Initialize nine_patch first so it adds the text_input as a child
	// clang-format off
	nine_patch(
		context,
		{
			.container_params{
				.layout = layout::pile
			},
			.padding_params = [&](){
				for(auto& b : params.padding_params.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}

				return std::move(params.padding_params);
			}(),
			.nine_patch_params = [&](){
				if(!params.nine_patch_params.nine_patch){
					params.nine_patch_params.nine_patch = context.get().loader().load<ruis::res::nine_patch>("ruis_npt_textfield_background"sv);
				}
				// TODO: set default disabled nine patch if not set

				return std::move(params.nine_patch_params);
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

utki::shared_ref<ruis::nine_patch_text_field> ruis::make::nine_patch_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::nine_patch_text_field::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::nine_patch_text_field>(
		context, //
		std::move(params),
		std::move(text)
	);
}
