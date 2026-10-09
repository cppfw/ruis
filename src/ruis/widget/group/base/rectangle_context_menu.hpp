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

#include "../../container.hpp"
#include "../../label/rectangle.hpp"
#include "../context_menu.hpp"

namespace ruis {

/**
 * @brief Context menu widget with a rectangle frame.
 * The menu items are displayed in a scrollable list surrounded by a frame (a filled, rounded,
 * bordered rectangle). The menu is min-wrap horizontally and vertically; when it is shown with a
 * size smaller than its natural size (e.g. by ruis::show_context_menu(), which clamps the menu
 * to the overlay's bounds) the list gets the clamped size and becomes scrollable instead of
 * having its content truncated.
 * See ruis::context_menu for the description of the context menu behavior.
 * This class has no public constructor; use ruis::touch::context_menu or derive from this
 * class with the protected constructor to create a context menu with a specific list widget.
 */
class rectangle_context_menu :
	public ruis::context_menu, //
	private container
{
public:
	struct parameters {
		ruis::list_widget::parameters list;
		/**
		 * @brief Parameters of the rectangle frame surrounding the menu.
		 * Undefined values are replaced with style defaults: corner radii are len_gap,
		 * fill color is color_background, stroke width is len_border, stroke color is
		 * color_primary, and the padding borders are
		 * {left = len_border, top = len_gap, right = len_border, bottom = len_gap}.
		 */
		ruis::rectangle::parameters rectangle;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	rectangle_context_menu(const rectangle_context_menu&) = delete;
	rectangle_context_menu& operator=(const rectangle_context_menu&) = delete;

	rectangle_context_menu(rectangle_context_menu&&) = delete;
	rectangle_context_menu& operator=(rectangle_context_menu&&) = delete;

	~rectangle_context_menu() override = default;

protected:
	/**
	 * @brief Construct a rectangle context menu with a custom list widget.
	 * This constructor is intended for derived classes. The list_factory is invoked with
	 * the list parameters whose provider is the decorated provider (see the
	 * ruis::context_menu description) wrapping the provider given in the 'list' parameter
	 * of params, so that the decoration and the item click handling are applied regardless
	 * of the list widget used.
	 * @param list_factory - a function creating the list widget which displays the menu items.
	 */
	rectangle_context_menu(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params, //
		std::function<utki::shared_ref<ruis::list_widget>(ruis::list_widget::parameters)> list_factory
	);
};

} // namespace ruis
