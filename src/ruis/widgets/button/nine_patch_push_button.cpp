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

#include "nine_patch_push_button.hpp"

#include "../../util/util.hpp"

using namespace ruis;

nine_patch_push_button::nine_patch_push_button( //
	utki::shared_ref<ruis::context> context,
	widget::parameters widget_params,
	button::parameters button_params,
	blending_widget::parameters blending_params,
	nine_patch::parameters nine_patch_params,
	nine_patch_button::parameters nine_patch_button_params,
	utki::span<const utki::shared_ref<ruis::widget>> children
) :
	widget(std::move(context), widget::all_parameters{std::move(widget_params)}),
	button(this->context, std::move(button_params)),
	push_button(this->context, button::parameters{}),
	nine_patch_button(
		this->context,
		{},
		std::move(blending_params),
		std::move(nine_patch_params),
		std::move(nine_patch_button_params),
		children
	)
{
	// TODO: load default pressed/unpressed nine-patches
}

nine_patch_push_button::nine_patch_push_button(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	button(this->context, desc),
	push_button(this->context, desc),
	nine_patch_button(this->context, desc)
{}

void nine_patch_push_button::on_pressed_change()
{
	this->nine_patch_button::on_pressed_change();
	this->push_button::on_pressed_change();
}
