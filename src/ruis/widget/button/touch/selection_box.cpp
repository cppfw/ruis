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

#include "selection_box.hpp"

#include "../../label/rectangle.hpp"
#include "../../label/text.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::touch;

namespace {
constexpr auto selection_label_id = "ruis_touch_selection_box_selection"sv;
} // namespace

selection_box::selection_box(
	utki::shared_ref<ruis::context> context, //
	ruis::selection_box::all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	container(this->context,
		{
			.container_params{
				.layout = ruis::layout::row
			}
		},
		{
			ruis::make::rectangle(this->context,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.color_params{
						.color = this->context.get().style().get_color_middleground()
					}
				}
			),
			ruis::make::text(this->context,
				{
					.layout_params{
						.align = {ruis::align::front, ruis::align::center}
					},
					.color_params{
						.color = this->context.get().style().get_color_text_normal()
					}
				},
				std::move(params.title)
			),
			ruis::make::container(this->context,
				{
					.layout_params{
						.weight = 1,
						.align = {ruis::align::back, ruis::align::center}
					},
					.widget_params{
						// TODO: refactor to avoid using lookup widget by id
						.id = std::string(selection_label_id)
					},
					.container_params{
						.layout = ruis::layout::pile
					}
				},
				{
					ruis::make::text(this->context,
					{
						.color_params{
							.color = this->context.get().style().get_color_text_normal()
						}
					},
					// TODO: should be 0th item from provider
					U"Hello world"s
				)
				}
			)
		}
	),
	// clang-format on
	ruis::selection_box(
		this->context, //
		this->get_widget_as<ruis::container>(selection_label_id),
		std::move(params.list_params)
	)
{}

utki::shared_ref<ruis::touch::selection_box> ruis::touch::make::selection_box(
	utki::shared_ref<context> context, //
	ruis::selection_box::all_parameters params
)
{
	return utki::make_shared<ruis::touch::selection_box>(
		std::move(context), //
		std::move(params)
	);
}
