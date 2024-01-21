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

#include "../../widget.hpp"

namespace morda {

/**
 * @brief Widget for intercepting mouse events.
 * From GUI script it can be instantiated as "mouse_proxy".
 */
class mouse_proxy : virtual public widget
{
public:
	mouse_proxy(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	mouse_proxy(const mouse_proxy&) = delete;
	mouse_proxy& operator=(const mouse_proxy&) = delete;

	mouse_proxy(mouse_proxy&&) = delete;
	mouse_proxy& operator=(mouse_proxy&&) = delete;

	~mouse_proxy() override = default;

	/**
	 * @brief Mouse button event signal.
	 * Emitted when mouse button event reaches this widget.
	 */
	std::function<bool(mouse_proxy& w, const mouse_button_event&)> mouse_button_handler;

	bool on_mouse_button(const mouse_button_event&) override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	std::function<bool(mouse_proxy& w, const mouse_move_event&)> mouse_move_handler;

	bool on_mouse_move(const mouse_move_event& event) override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	std::function<void(mouse_proxy& w, unsigned pointer_id)> hover_change_handler;

	void on_hover_change(unsigned pointer_id) override;
};

} // namespace morda
