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

#include <functional>

#include <ruis/util/widget_list.hpp>
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
 * @brief A ruis::list_provider which provides context menu item widgets.
 * Each widget supplied to the provider is wrapped with a ruis::click_proxy and a
 * ruis::mouse_proxy which show a background of color_special color while the item
 * is pressed and of color_highlight color while the item is hovered.
 */
class context_menu_provider : public ruis::list_provider
{
	ruis::widget_list items;

public:
	/**
	 * @brief Item click handler.
	 * Invoked when a menu item is clicked.
	 * The context_menu::show() function sets this handler to close the menu.
	 */
	std::function<void()> on_item_click;

	context_menu_provider(
		const utki::shared_ref<ruis::context>& context, //
		ruis::widget_list widgets
	);

	size_t count() const noexcept override;
	utki::shared_ref<ruis::widget> get_widget(size_t index) const override;

private:
	utki::shared_ref<ruis::widget> wrap_item(
		const utki::shared_ref<ruis::widget>& content, //
		bool is_last
	);
};

/**
 * @brief Shows a context menu near the given anchor widget.
 * The context menu is a context_menu widget (a ruis::list_widget) whose items
 * are supplied by a context_menu_provider constructed from the given widgets.
 * Each supplied widget is wrapped with a ruis::click_proxy and a ruis::mouse_proxy
 * which show a background of color_special color while the item is pressed and of
 * color_highlight color while the item is hovered, and closes the menu on click.
 * Consecutive menu items are separated by a thin line of color_secondary color.
 * The menu is min-wrap horizontally, and min-wrap vertically, but clamped to the
 * screen so that it fits even if the list is longer than the screen
 * (in which case the menu can be scrolled).
 * The menu is shown on the nearest ruis::overlay ancestor of the anchor widget
 * and is automatically closed when a click happens outside of it.
 * @param anchor - the widget near which the menu should be shown.
 * @param widgets - the list of widgets to use as menu items.
 */
void show(
	ruis::widget& anchor, //
	ruis::widget_list widgets
);

} // namespace context_menu
