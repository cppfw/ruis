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

#include "../container.hpp"

namespace ruis {

/**
 * @brief Overlay container for displaying widgets on top of anything.
 * Overlay container is used for displaying topmost widgets like context menus, hint popups etc.
 * Essentially, the overlay is a simple container with pile-layout which keeps track of open popup widgets.
 */
class overlay : public container
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
	};

	overlay(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	overlay(const overlay&) = delete;
	overlay& operator=(const overlay&) = delete;

	overlay(overlay&&) = delete;
	overlay& operator=(overlay&&) = delete;

	~overlay() override = default;

	/**
	 * @brief Showing context menu.
	 * This function adds the popup widget to the overlay as the topmost widget.
	 * The given popup widget is wrapped into special overhead container to make it properly positioned
	 * on the screen and to handle mouse clicks outside of the popup widget
	 * (i.e. close the popup in case of mouse click outside).
	 * @param popup - popup widget to show.
	 * @param pos - position of top left corner of the popup within the overlay container.
	 * @return the final widget added to the overlay. This widget can be used to later close the particular popup
	 * by just removing the widget from its parent.
	 */
	utki::shared_ref<widget> show_popup(
		utki::shared_ref<widget> popup, //
		vec2 pos
	);

	/**
	 * @brief Close all popups.
	 */
	void close_all_popups();
};

namespace make {
inline utki::shared_ref<ruis::overlay> overlay(
	utki::shared_ref<ruis::context> context, //
	ruis::overlay::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::overlay>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
} // namespace make

} // namespace ruis
