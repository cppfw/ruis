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
				.layout{
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.params = std::move(params.params.text_input)
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
		std::move(params.layout),
		std::move(params.widget)
	),
	// Initialize nine_patch first so it adds the text_input as a child
	// clang-format off
	nine_patch(
		context,
		{
			.params = [&](){
				if(auto& l = params.params.nine_patch.padding.container.layout; !l){
					l = layout::pile;
				}

				for(auto& b : params.params.nine_patch.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}

				if(auto& np = params.params.nine_patch.specific.source; !np){
					np = context.get().loader().load<ruis::res::nine_patch>("ruis_npt_textfield_background"sv);
				}

				return std::move(params.params.nine_patch);
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
