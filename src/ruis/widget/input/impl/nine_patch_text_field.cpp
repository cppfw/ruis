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
					.dims = {ruis::dim::fill, ruis::dim::max},
					.weight = 1
				},
				.params = std::move(params.params.text_input)
			},
			std::move(text)
		),
		make_clear_button(context, params.params.text_field.clear_button)
		// clang-format on
	)
{}

nine_patch_text_field::nine_patch_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text_input> text_input, //
	std::shared_ptr<ruis::image_push_button> clear_button_widget
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// Initialize nine_patch first so it adds the text_input (and the clear button,
	// if any) as its children
	// clang-format off
	nine_patch(
		context,
		{
			.params = [&](){
				if(auto& l = params.params.nine_patch.padding.container.layout; !l){
					// The text_input and the clear button (if any) are arranged side-by-side
					// in a row.
					l = layout::row;
				}

				for(auto& b : params.params.nine_patch.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap_small();
					}
				}

				if(auto& np = params.params.nine_patch.specific.source; !np){
					np = context.get().loader().load<ruis::res::nine_patch>("ruis_npt_textfield_background"sv);
				}

				return std::move(params.params.nine_patch);
			}()
		},
		make_content_children(text_input, clear_button_widget)
	),
	// clang-format on
	text_field(
		context, //
		text_input.get()
	),
	clear_button(std::move(clear_button_widget))
{
	if (this->clear_button) {
		// Pressing the clear button clears the text input.
		this->clear_button->click_handler = [this](ruis::push_button&) {
			this->get_text_input().clear();
			this->get_text_input().set_cursor_index(0);
		};
	}
}

std::shared_ptr<ruis::image_push_button> nine_patch_text_field::make_clear_button(
	const utki::shared_ref<ruis::context>& context, //
	bool enabled
)
{
	if (!enabled) {
		return nullptr;
	}

	// clang-format off
	return ruis::make::image_push_button(
		context, //
		{
			.layout_params{
				// Fill the container vertically and take the minimum horizontal space,
				// keeping the image's aspect ratio, so the button ends up as a square
				// whose side equals the text field's content height.
				.dims = {ruis::dim::min, ruis::dim::fill}
			},
			.params{
				.image{
					.color{
						.normal = context.get().style().get_color_secondary()
					},
					.specific{
						.keep_aspect_ratio = true
					}
				},
				.image_button{
					.unpressed_image = context.get().loader().load<ruis::res::image>("ruis_img_cross"sv),
					.pressed_image   = context.get().loader().load<ruis::res::image>("ruis_img_cross"sv)
				}
			}
		}
	);
	// clang-format on
}

widget_list nine_patch_text_field::make_content_children(
	const utki::shared_ref<ruis::text_input>& text_input, //
	const std::shared_ptr<ruis::image_push_button>& clear_button
)
{
	widget_list children;
	children.emplace_back(text_input);
	if (clear_button) {
		children.emplace_back(utki::shared_ref<ruis::image_push_button>(clear_button));
	}
	return children;
}

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
