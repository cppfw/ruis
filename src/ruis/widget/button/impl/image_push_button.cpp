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

#include "image_push_button.hpp"

using namespace ruis;

void image_push_button::on_pressed_change()
{
	this->image_button::on_pressed_change();
	this->push_button::on_pressed_change();
}

image_push_button::image_push_button( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button( //
		this->context,
		std::move(params.button_params)
	),
	push_button( //
		this->context,
		button::parameters{}
	),
	image_button( //
		this->context,
		std::move(params.blending_params),
		std::move(params.image_params),
		std::move(params.image_button_params)
	)
{}

utki::shared_ref<ruis::image_push_button> ruis::make::image_push_button(
	utki::shared_ref<ruis::context> context, //
	ruis::image_push_button::all_parameters params
)
{
	return utki::make_shared<ruis::image_push_button>(
		std::move(context), //
		std::move(params)
	);
}
