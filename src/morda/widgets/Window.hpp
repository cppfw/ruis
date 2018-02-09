#pragma once

#include "label/Text.hpp"
#include "label/Color.hpp"

#include "core/container/Pile.hpp"
#include "core/proxy/MouseProxy.hpp"
#include "core/container/Table.hpp"

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
	std::uint32_t titleBgColorTopmost;
	std::uint32_t titleBgColorNonTopmost;
	
	std::shared_ptr<Text> title;
	
	std::shared_ptr<Pile> contentArea;
	
	std::shared_ptr<Widget> tBorder;
	std::shared_ptr<Widget> lBorder;
	std::shared_ptr<Widget> rBorder;
	std::shared_ptr<Widget> bBorder;
	
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
	Window(const stob::Node* chain = nullptr);
	
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

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerId)override;
	
	void setBackground(std::shared_ptr<Widget> w);
	
	void layOut() override;
	
	/**
	 * @brief Set window borders.
	 * @param borders - borders in pixels.
	 */
	void setBorders(Sidesr borders);
};

}
