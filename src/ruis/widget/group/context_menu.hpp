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

#include "../../util/widget_list.hpp"
#include "../base/list_widget.hpp"
#include "../container.hpp"

namespace ruis::touch {
class list;
} // namespace ruis::touch

namespace ruis {

/**
 * @brief Context menu widget.
 * The context menu is a styled menu of items supplied by a ruis::list_provider (see the 'list'
 * parameter). Each item widget provided by the list_provider is wrapped with a ruis::click_proxy
 * and a ruis::mouse_proxy which show a background of color_highlight color while the item is
 * pressed and of color_secondary color while the item is hovered, and close the menu when the
 * item is clicked. Consecutive menu items are separated by a thin line of color_secondary color.
 * The menu items are displayed in a scrollable list surrounded by a frame (a filled, rounded,
 * bordered rectangle). The menu is min-wrap horizontally and vertically; when it is shown with a
 * size smaller than its natural size (e.g. by ruis::show_context_menu(), which clamps the menu
 * to the overlay's bounds) the list gets the clamped size and becomes scrollable instead of
 * having its content truncated.
 * The context menu is meant to be shown on an ruis::overlay, e.g. with the
 * ruis::show_context_menu() function. Clicking a menu item closes the menu (removes the menu
 * widget from its parent); when the menu is shown with ruis::show_context_menu() it is also
 * closed when a click happens outside of it.
 */
class context_menu :
	public virtual widget, //
	private container
{
	// the scrollable list of the menu items
	utki::shared_ref<ruis::touch::list> list;

	// the sum of the top and bottom borders of the frame,
	// used to fit the list into the menu's size
	real frame_v_border;

public:
	struct parameters {
		ruis::list_widget::parameters list;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	context_menu(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

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

private:
	void on_lay_out() override;

	// Fits the list into the given menu size (minus the frame's vertical borders) by setting
	// the list's size to the size needed to display the whole content without truncation,
	// clamped to the menu size.
	void fit_list(const vec2& menu_size);
};

namespace make {
/**
 * @brief Construct 'context_menu' widget.
 * @param context - ruis context.
 * @param params - 'context_menu' widget parameters.
 * @return newly constructed 'context_menu' widget.
 */
utki::shared_ref<ruis::context_menu> context_menu(
	const utki::shared_ref<context>& context, //
	context_menu::all_parameters params
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
