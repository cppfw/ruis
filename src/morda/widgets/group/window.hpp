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

#include "../label/color.hpp"
#include "../label/text.hpp"
#include "../proxy/mouse_proxy.hpp"

namespace morda {

/**
 * @brief Window widget.
 * Window has borders, caption bar and can contain child widgets.
 * Window can be resized by dragging one of the borders or corners of the window.
 * Window can be moved by dragging caption bar.
 * From GUI script it can be instantiated as "window".
 * @param title - text of the window title.
 * @param look - style settings.
 */
class window : virtual public widget, private container
{
	morda::vector2 empty_min_dim; // minimal dimension of empty window

	std::shared_ptr<color> title_bg;
	uint32_t title_bg_color_topmost = 0xffff0000;
	uint32_t title_bg_color_non_topmost = 0xff808080;

	std::shared_ptr<text> title;

	std::shared_ptr<container> content_area;

	std::shared_ptr<widget> lt_border;
	std::shared_ptr<widget> rt_border;
	std::shared_ptr<widget> t_border;
	std::shared_ptr<widget> l_border;
	std::shared_ptr<widget> r_border;
	std::shared_ptr<widget> b_border;
	std::shared_ptr<widget> lb_border;
	std::shared_ptr<widget> rb_border;

	using cursor_iter = decltype(std::declval<mouse_cursor_manager>().push(mouse_cursor::arrow));
	cursor_iter caption_cursor_iter;
	cursor_iter lt_border_cursor_iter;
	cursor_iter rt_border_cursor_iter;
	cursor_iter t_border_cursor_iter;
	cursor_iter l_border_cursor_iter;
	cursor_iter r_border_cursor_iter;
	cursor_iter b_border_cursor_iter;
	cursor_iter lb_border_cursor_iter;
	cursor_iter rb_border_cursor_iter;

	bool mouse_captured = false;
	morda::vector2 capture_point;

	void setup_widgets();

	void update_topmost();

public:
	window(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	window(const window&) = delete;
	window& operator=(const window&) = delete;

	void set_title(const std::string& str);

	/**
	 * @brief Get window content area.
	 * This is where child widgets are stored.
	 * @return Window content area.
	 */
	container& content()
	{
		return *this->content_area;
	}

	/**
	 * @brief Move the window to the Z order top within its parent.
	 */
	void move_to_top();

	/**
	 * @brief Check if the widget is currently topmost within its parent.
	 * @return true if the widget is topmost.
	 * @return false otherwise.
	 */
	bool is_topmost() const noexcept;

	bool on_mouse_button(const mouse_button_event& event) override;

	bool on_mouse_move(const mouse_move_event& event) override;

	void set_background(const utki::shared_ref<widget>& w);

	void on_lay_out() override;

	/**
	 * @brief Set window borders.
	 * @param borders - borders in pixels.
	 */
	void set_borders(sides<real> borders);
};

} // namespace morda
