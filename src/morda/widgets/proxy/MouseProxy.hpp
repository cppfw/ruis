#pragma once

#include "../widget.hpp"


namespace morda{

/**
 * @brief Widget for intercepting mouse events.
 * From GUI script it can be instantiated as "MouseProxy".
 */
class MouseProxy : virtual public Widget{
public:
	MouseProxy(const puu::forest& desc);

	MouseProxy(const MouseProxy&) = delete;
	MouseProxy& operator=(const MouseProxy&) = delete;

	/**
	 * @brief Mouse button event signal.
	 * Emitted when mouse button event reaches this widget.
	 */
	std::function<bool (Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)> mouseButton;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	std::function<bool (Widget& widget, const morda::Vec2r& pos, unsigned pointerId)> mouseMove;

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerId)override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	std::function<void(Widget& widget, unsigned pointerID)> hoverChanged;

	void on_hover_changed(unsigned pointerID)override;
};

}
