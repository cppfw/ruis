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
#include <vector>

#include <ruis/util/localization.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>

namespace context_menu {

/**
 * @brief A single action shown in the context menu.
 */
struct action {
	ruis::string label;
	std::function<void()> on_click;
};

/**
 * @brief Shows a context menu near the given anchor widget.
 * The context menu is a vertical list of actions built with a plain ruis::column
 * (not a ruis::list), framed with a rectangle that has a border.
 * The menu is min-wrap horizontally, and min-wrap vertically, but clamped to the
 * screen so that it fits even if the list is longer than the screen (a scroll area
 * is planned to be added inside the frame in the future).
 * The menu is shown on the nearest ruis::overlay ancestor of the anchor widget
 * and is automatically closed when a click happens outside of it.
 * @param anchor - the widget near which the menu should be shown.
 * @param actions - the list of actions to show in the menu.
 */
void show(
	ruis::widget& anchor, //
	std::vector<action> actions
);

} // namespace context_menu
