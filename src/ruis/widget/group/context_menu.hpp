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

#include "../base/list_widget.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Base class for context menu widgets.
 * A context menu is a menu of items supplied by a ruis::list_provider (see the 'list'
 * parameter). Each item widget provided by the list_provider is wrapped with a ruis::click_proxy
 * and a ruis::mouse_proxy which show a background of color_highlight color while the item is
 * pressed and of color_secondary color while the item is hovered, and close the menu when the
 * item is clicked. Consecutive menu items are separated by a thin line of color_secondary color.
 * The context menu is meant to be shown on an ruis::overlay, e.g. with the
 * ruis::show_context_menu() function. Clicking a menu item closes the menu (removes the menu
 * widget from its parent); when the menu is shown with ruis::show_context_menu() it is also
 * closed when a click happens outside of it.
 * This class has no public constructor; use ruis::touch::context_menu or derive from this
 * class with the protected constructor to create a context menu.
 */
class context_menu : public virtual widget
{
public:
	struct parameters {
		ruis::list_widget::parameters list;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	context_menu(const context_menu&) = delete;
	context_menu& operator=(const context_menu&) = delete;

	context_menu(context_menu&&) = delete;
	context_menu& operator=(context_menu&&) = delete;

	~context_menu() override = default;

	/**
	 * @brief Item click handler.
	 * Invoked when a menu item is clicked, right before the menu is closed.
	 * @param index - the index of the clicked menu item.
	 */
	std::function<void(size_t index)> on_item_click;

	/**
	 * @brief Close the menu.
	 * Removes the menu widget from its parent.
	 */
	void close();

protected:
	/**
	 * @brief Construct a context menu.
	 * This constructor is intended for derived classes.
	 */
	explicit context_menu(const utki::shared_ref<ruis::context>& context);
};

namespace make {
/**
 * @brief Construct a context menu widget.
 * @param context - ruis context.
 * @param params - context menu parameters.
 * @return newly constructed context menu widget.
 */
utki::shared_ref<ruis::context_menu> context_menu(
	const utki::shared_ref<ruis::context>& context, //
	ruis::context_menu::all_parameters params
);
} // namespace make

/**
 * @brief Show a context menu near the given anchor widget.
 * The given menu widget (which can be any widget) is wrapped into a popup (see
 * ruis::overlay::show_popup) which handles mouse clicks outside of the menu (closes the menu on
 * the first click outside of it), and is shown on the nearest ruis::overlay ancestor of the
 * anchor widget right below it, right-aligned with it, clamped to the overlay's bounds.
 * @param anchor - the widget near which the menu should be shown.
 * @param menu - the widget to show as a context menu.
 * @return the popup widget added to the overlay. Removing the widget from its parent closes the menu.
 */
utki::shared_ref<ruis::widget> show_context_menu(
	ruis::widget& anchor, //
	utki::shared_ref<ruis::widget> menu
);

} // namespace ruis
