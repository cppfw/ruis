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

using namespace ruis::length_literals;

utki::shared_ref<ruis::push_button> m::push_button(
	utki::shared_ref<ruis::context> context,
	ruis::rectangle_push_button::all_parameters params,
	ruis::widget_list contents
)
{
	params.padding_params.borders = 5_pp;
	params.rectangle_params.corner_radii = 5_pp;

	return ruis::make::rectangle_push_button(
		std::move(context), //
		std::move(params),
		std::move(contents)
	);
}
