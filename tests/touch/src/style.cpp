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

#include "style.hpp"

#include <ruis/widget/button/impl/rectangle_push_button.hpp>

using namespace ruis::length_literals;

utki::shared_ref<ruis::push_button> m::push_button(
	utki::shared_ref<ruis::context> context, //
	ruis::container::all_parameters params,
	ruis::widget_list contents
)
{
	// clang-format off
	return ruis::make::rectangle_push_button(
		std::move(context),
		{
            .layout_params = std::move(params.layout_params),
            .widget_params = std::move(params.widget_params),
            .container_params = std::move(params.container_params),
            .padding_params{
                .borders = {5_pp}
            },
            .rectangle_params{
                .corner_radii = {5_pp}
            }
        },
		std::move(contents)
	);
	// clang-format on
}
