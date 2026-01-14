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

#include "radio_button.hpp"

#include "../../../context.hpp"
#include "../../label/image.hpp"
#include "../choice_group.hpp"

using namespace std::string_view_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

radio_button::radio_button(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button(
		this->context, //
		std::move(params.button_params)
	),
	toggle_button(this->context),
	choice_button(this->context),
	// clang-format off
	container(this->context,
		{
			.container_params{
				.layout = layout::pile
			}
		},
		{
			m::image(this->context,
				{
					.image_params{
						.img = this->context.get().loader().load<res::image>("ruis_img_radiobutton_bg"sv)
					}
				}
			),
			m::image(this->context,
				{
					.widget_params{
						.visible = this->is_pressed()
					},
					.image_params{
						.img = this->context.get().loader().load<res::image>("ruis_img_radiobutton_tick")
					}
				}
			)
		}
	),
	// clang-format on
	check_widget(*this->children().rbegin())
{}

void radio_button::on_pressed_change()
{
	this->choice_button::on_pressed_change();
	this->check_widget.get().set_visible(this->is_pressed());
	this->clear_cache();
}
