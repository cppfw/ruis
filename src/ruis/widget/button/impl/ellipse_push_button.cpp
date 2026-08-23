/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "ellipse_push_button.hpp"

using namespace ruis;

void ellipse_push_button::on_pressed_change()
{
	this->ellipse_button::on_pressed_change();
	this->push_button::on_pressed_change();
}

ellipse_push_button::ellipse_push_button(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	ruis::widget_list contents
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params) //
	),
	button(this->context, {}),
	push_button(this->context, {}),
	ellipse_button( //
		this->context,
		std::move(params.container_params),
		std::move(params.padding_params),
		std::move(params.ellipse_button_params),
		std::move(contents) //
	)
{}

namespace ruis::make {

utki::shared_ref<ruis::ellipse_push_button> ellipse_push_button(
	utki::shared_ref<ruis::context> context,
	ruis::ellipse_push_button::all_parameters params,
	ruis::widget_list contents
)
{
	return utki::make_shared<ruis::ellipse_push_button>(
		std::move(context), //
		std::move(params),
		std::move(contents)
	);
}

} // namespace ruis::make