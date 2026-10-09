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

#include "context_menu.hpp"

#include <utki/shared.hpp>

#include "list.hpp"

ruis::touch::context_menu::context_menu(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_context_menu::all_parameters params
) :
	// ruis::widget is a virtual base of ruis::context_menu, so it has to be initialized
	// by the most derived class
	widget( //
		context, //
		std::move(params.layout_params), //
		std::move(params.widget) //
	),
	ruis::rectangle_context_menu( //
		context, //
		std::move(params), //
		[&context](ruis::list_widget::parameters p) -> utki::shared_ref<ruis::list_widget> {
			// the default list: a scrollable ruis::touch::list
			// clang-format off
			auto list_params = ruis::touch::list::all_parameters{
				.layout_params{
					// max width: the list fills the menu width when the menu is shown with a
					// concrete width, and wraps its content when shown with its natural size
					.dims = {ruis::dim::max, ruis::dim::min}
				},
				.params{
					.specific{
						std::move(p)
					}
				}
			};
			// clang-format on
			return ruis::touch::make::list(context, std::move(list_params));
		}
	)
{}

utki::shared_ref<ruis::touch::context_menu> ruis::touch::make::context_menu(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_context_menu::all_parameters params
)
{
	return utki::make_shared<ruis::touch::context_menu>(
		context, //
		std::move(params)
	);
}
