/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "check_box.hpp"

#include "../../../context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis;

check_box::check_box(utki::shared_ref<ruis::context> context, all_parameters params) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button( //
		this->context,
		std::move(params.button_params)
	),
	toggle_button(this->context),
	// clang-format off
	nine_patch(this->context,
		{
			.container_params = {
				.layout = layout::pile
			},
			.nine_patch_params = [&](){
				auto p = std::move(params.nine_patch_params);
				if(!p.nine_patch){
					p.nine_patch = context.get().loader.load<res::nine_patch>("ruis_npt_checkbox_bg"sv);
				}
				return p;
			}()
		},
		{
			make::image(this->context,
				{
					.widget_params = {
						.id = "ruis_checkbox_check"s
					},
					.image_params = [&](){
						auto p = std::move(params.image_params);
						if(!p.img){
							p.img = context.get().loader.load<res::image>("ruis_img_checkbox_tick"sv);
						}
						return p;
					}()
				}
			)
		}
	),
	tick_widget(this->content().get_widget_as<image>("ruis_checkbox_check"sv))
// clang-format on
{
	this->tick_widget.set_visible(this->is_pressed());
}

check_box::check_box(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	button(this->context, desc),
	toggle_button(this->context, desc),
	nine_patch(
		this->context,
		{.container_params = {.layout = layout::pile}},
		{make::image(
			this->context,
			{.widget_params = {.id = "ruis_checkbox_check"s},
			 .image_params = {.img = this->context.get().loader.load<res::image>("ruis_img_checkbox_tick"sv)}}
		)}
	),
	tick_widget(this->content().get_widget_as<image>("ruis_checkbox_check"sv))
{
	this->tick_widget.set_visible(this->is_pressed());
	this->set_nine_patch(this->context.get().loader.load<res::nine_patch>("ruis_npt_checkbox_bg").to_shared_ptr());
}

void check_box::on_pressed_change()
{
	this->tick_widget.set_visible(this->is_pressed());
	this->clear_cache();
	this->toggle_button::on_pressed_change();
}
