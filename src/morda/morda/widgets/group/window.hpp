#pragma once

#include "../label/text.hpp"
#include "../label/color.hpp"

#include "pile.hpp"

#include "../proxy/mouse_proxy.hpp"

namespace morda{

/**
 * @brief Window widget.
 * Window has borders, caption bar and can contain child widgets laid out as in pile container.
 * Window can be resized by dragging one of the borders or corners of the window.
 * Window can be moved by dragging caption bar.
 * From GUI script it can be instantiated as "window".
 * @param title - text of the window title.
 * @param look - style settings.
 */
class window :
		virtual public widget,
		private pile
{
	morda::vector2 emptyMinDim; // minimal dimension of empty window

	std::shared_ptr<color> titleBg;
	uint32_t titleBgColorTopmost = 0xffff0000;
	uint32_t titleBgColorNonTopmost = 0xff808080;

	std::shared_ptr<text> title;

	std::shared_ptr<pile> contentArea;

	std::shared_ptr<widget> ltBorder;
	std::shared_ptr<widget> rtBorder;
	std::shared_ptr<widget> tBorder;
	std::shared_ptr<widget> lBorder;
	std::shared_ptr<widget> rBorder;
	std::shared_ptr<widget> bBorder;
	std::shared_ptr<widget> lbBorder;
	std::shared_ptr<widget> rbBorder;

	typedef decltype(std::declval<mouse_cursor_manager>().push(mouse_cursor::arrow)) cursor_iter;
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
	morda::vector2 capturePoint;

	void setup_widgets();

	void updateTopmost();

public:
	window(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	window(const window&) = delete;
	window& operator=(const window&) = delete;

	void set_title(const std::string& str);

	/**
	 * @brief Get window content area.
	 * This is where child widgets are stored.
	 * @return Window content area.
	 */
	pile& content(){
		return *this->contentArea;
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
	bool is_topmost()const noexcept;

	bool on_mouse_button(const mouse_button_event& event)override;

	bool on_mouse_move(const mouse_move_event& event)override;

	void set_background(std::shared_ptr<widget> w);

	void lay_out() override;

	/**
	 * @brief Set window borders.
	 * @param borders - borders in pixels.
	 */
	void set_borders(sides<real> borders);
};

}
