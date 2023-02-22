/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "pile.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 * Overlay container is used for displaying topmost widgets like context menus, hint popups etc.
 * Essentially, the overlay is a simple pile container which keeps track of open overlay widgets.
 * From GUI scripts it can be instantiated as "overlay".
 */
class overlay : public pile{
public:
	overlay(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);
	
	overlay(const overlay&) = delete;
	overlay& operator=(const overlay&) = delete;
	
	/**
	 * @brief Showing context menu.
	 * This function adds the context menu widget to the overlay as the topmost widget.
	 * The given context menu widget is wrapped into special overhead container to make it properly positioned
	 * on the screen and to handle mouse clicks outside of the context menu widget
	 * (i.e. close the context menu in case of mouse click outside).
	 * @param menu - context menu widget.
	 * @param pos - position of top left corner of the context menu within the overlay container.
	 * @return the final widget added to the overlay pile. This widget can be used to later close the particular context menu
	 *         by just removing the widget from its parent.
	 */
	utki::shared_ref<widget> show_context_menu(const utki::shared_ref<widget>& menu, vector2 pos);
	
	/**
	 * @brief Close all context menus.
	 */
	void close_all_context_menus();
};

}
