/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "image_toggle.hpp"

#include "../../../context.hpp"
#include "../../../util/util.hpp"

using namespace ruis;

image_toggle::image_toggle(
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
	image_button(
		this->context,
		std::move(params.blending_params), //
		std::move(params.image_params),
		std::move(params.image_button_params)
	)
{}

utki::shared_ref<ruis::image_toggle> ruis::make::image_toggle(
	utki::shared_ref<ruis::context> context, //
	ruis::image_toggle::all_parameters params
)
{
	return utki::make_shared<ruis::image_toggle>(
		std::move(context), //
		std::move(params)
	);
}
