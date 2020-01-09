#pragma once

#include "../label/Text.hpp"
#include "../label/Color.hpp"

#include "Pile.hpp"

#include "../proxy/MouseProxy.hpp"

namespace morda{


/**
 * @brief Window widget.
 * Window has borders, caption bar and can contain child widgets laid out as in Pile container.
 * Window can be resized by dragging one of the borders or corners of the window.
 * Window can be moved by dragging caption bar.
 * From GUI script it can be instantiated as "Window".
 * @param title - text of the window title.
 * @param look - style settings.
 */
class Window :
		virtual public Widget,
		private Pile
{
	morda::Vec2r emptyMinDim;//minimal dimension of empty window

	std::shared_ptr<Color> titleBg;
	std::uint32_t titleBgColorTopmost = 0xffff0000;
	std::uint32_t titleBgColorNonTopmost = 0xff808080;

	std::shared_ptr<Text> title;

	std::shared_ptr<Pile> contentArea;

	std::shared_ptr<Widget> ltBorder;
	std::shared_ptr<Widget> rtBorder;
	std::shared_ptr<Widget> tBorder;
	std::shared_ptr<Widget> lBorder;
	std::shared_ptr<Widget> rBorder;
	std::shared_ptr<Widget> bBorder;
	std::shared_ptr<Widget> lbBorder;
	std::shared_ptr<Widget> rbBorder;

	bool captionCaptured = false;
	bool leftTopResizeCaptured = false;
	bool leftBottomResizeCaptured = false;
	bool rightTopResizeCaptured = false;
	bool rightBottomResizeCaptured = false;
	bool leftResizeCaptured = false;
	bool rightResizeCaptured = false;
	bool topResizeCaptured = false;
	bool bottomResizeCaptured = false;

	morda::Vec2r capturePoint;

	void setupWidgets();

	void updateTopmost();

public:
	Window(const puu::forest& desc);
	Window(const stob::Node* chain) : Window(stob_to_puu(chain)){}

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void setTitle(const std::string& str);

	/**
	 * @brief Get window content area.
	 * This is where child widgets are stored.
	 * @return Window content area.
	 */
	Pile& content(){
		return *this->contentArea;
	}

	/**
	 * @brief Move the window to the Z order top within its parent.
	 */
	void makeTopmost();

	/**
	 * @brief Check if the widget is currently topmost within its parent.
	 * @return true if the widget is topmost.
	 * @return false otherwise.
	 */
	bool isTopmost()const noexcept;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override;

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerId)override;

	void setBackground(std::shared_ptr<Widget> w);

	void lay_out() override;

	/**
	 * @brief Set window borders.
	 * @param borders - borders in pixels.
	 */
	void setBorders(Sidesr borders);
};

}
