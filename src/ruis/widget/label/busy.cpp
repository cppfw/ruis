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

#include "busy.hpp"

using namespace ruis;

busy::busy(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	spinner(
		this->context, //
		// clang-format off
		{
			.image_params = std::move(params.image_params),
			.blending_params = std::move(params.blending_params)
		}
		// clang-format on
	)
{
	if (!this->get_image()) {
		this->set_image(this->context.get().loader().load<ruis::res::image>("ruis_img_busy"));
	}
}

void busy::set_active(bool active)
{
	this->set_visible(active);
	this->spinner::set_active(active);
}
