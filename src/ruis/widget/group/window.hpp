/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../../util/length.hpp"
#include "../label/rectangle.hpp"
#include "../label/text.hpp"
#include "../proxy/mouse_proxy.hpp"

namespace ruis {

/**
 * @brief Window widget.
 * Window has borders, caption bar and can contain child widgets.
 * Window can be resized by dragging one of the borders or corners of the window.
 * Window can be moved by dragging caption bar.
 * From GUI script it can be instantiated as "window".
 * @param title - text of the window title.
 * @param look - style settings.
 */
// TODO: derive from frame_widget
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class window :
	virtual public widget, //
	private container
{
	ruis::vector2 empty_min_dim; // minimal dimension of empty window

	std::shared_ptr<rectangle> title_bg;
	constexpr static auto default_title_bg_color_topmost = 0xffad9869;
	uint32_t title_bg_color_topmost = default_title_bg_color_topmost;
	constexpr static auto default_title_bg_color_non_topmost = 0xff505050;
	uint32_t title_bg_color_non_topmost = default_title_bg_color_non_topmost;

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
	ruis::vector2 capture_point;

	void setup_widgets();

	void update_topmost();

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		string title;
		std::shared_ptr<widget> background; // TODO: only nine_patch?
		sides<length> borders;
	};

	window(
		utki::shared_ref<ruis::context> c,
		all_parameters params,
		utki::span<const utki::shared_ref<ruis::widget>> children
	);

	window(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	window(const window&) = delete;
	window& operator=(const window&) = delete;

	window(window&&) = delete;
	window& operator=(window&&) = delete;

	~window() override = default;

	void set_title(const std::string& str);

	void set_title(std::u32string str);

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

	void set_background(utki::shared_ref<widget> w);

	void on_lay_out() override;

	/**
	 * @brief Set window borders.
	 * @param borders - window borders.
	 */
	void set_borders(sides<length> borders);
};

namespace make {
/**
 * @brief Construct 'window' widget.
 * Default content layout is pile.
 * @param context - ruis context.
 * @param params - 'window' widget parameters.
 * @param contents - contents of the 'window' widget.
 * @return newly constructed 'window' widget.
 */
utki::shared_ref<ruis::window> window(
	utki::shared_ref<context> context,
	window::all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> contents
);
} // namespace make

} // namespace ruis
