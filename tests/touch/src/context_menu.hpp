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

#include <ruis/widget/base/list_widget.hpp>
#include <ruis/widget/group/touch/list.hpp>

namespace context_menu {

/**
 * @brief Context menu widget.
 * This is a ruis::list_widget (based on ruis::touch::list) which gets its items
 * from a ruis::list_provider.
 */
class context_menu : public ruis::touch::list
{
public:
	using all_parameters = ruis::touch::list::all_parameters;

	context_menu(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);
};

/**
 * @brief Shows a context menu near the given anchor widget.
 * The context menu is a context_menu widget (a ruis::list_widget) whose items
 * are supplied by the given list_provider.
 * Each item widget provided by the list_provider is wrapped with a
 * ruis::click_proxy and a ruis::mouse_proxy which show a background of
 * color_highlight color while the item is pressed and of color_secondary color
 * while the item is hovered, and closes the menu on click.
 * Consecutive menu items are separated by a thin line of color_secondary color.
 * The menu is min-wrap horizontally, and min-wrap vertically, but clamped to the
 * screen so that it fits even if the list is longer than the screen
 * (in which case the menu can be scrolled).
 * The menu is shown on the nearest ruis::overlay ancestor of the anchor widget
 * and is automatically closed when a click happens outside of it.
 * @param anchor - the widget near which the menu should be shown.
 * @param provider - the list provider to supply menu item widgets with.
 *                   Takes ownership of the provider.
 */
void show(
	ruis::widget& anchor, //
	utki::unique_ref<ruis::list_provider> provider
);

} // namespace context_menu
