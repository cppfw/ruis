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

#pragma once

#include "../base/rectangle_context_menu.hpp"

namespace ruis::touch {

/**
 * @brief Context menu widget which displays the menu items in a scrollable ruis::touch::list.
 * See ruis::context_menu for the description of the context menu.
 */
class context_menu : public ruis::rectangle_context_menu
{
public:
	/**
	 * @brief Construct a context menu.
	 * The menu items are supplied by the ruis::list_provider given in the 'list' parameter;
	 * they are displayed in a scrollable ruis::touch::list which is created by this constructor.
	 */
	context_menu(
		const utki::shared_ref<ruis::context>& context, //
		ruis::rectangle_context_menu::all_parameters params
	);

	context_menu(const context_menu&) = delete;
	context_menu& operator=(const context_menu&) = delete;

	context_menu(context_menu&&) = delete;
	context_menu& operator=(const context_menu&&) = delete;

	~context_menu() override = default;
};

namespace make {
/**
 * @brief Construct 'context_menu' widget.
 * @param context - ruis context.
 * @param params - 'context_menu' widget parameters.
 * @return newly constructed 'context_menu' widget.
 */
utki::shared_ref<ruis::touch::context_menu> context_menu(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_context_menu::all_parameters params
);
} // namespace make

} // namespace ruis::touch
