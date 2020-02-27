#pragma once

#include "../widget.hpp"

namespace morda{

/**
 * @brief Widget for intercepting mouse events.
 * From GUI script it can be instantiated as "mouse_proxy".
 */
class mouse_proxy : virtual public widget{
public:
	mouse_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc);

	mouse_proxy(const mouse_proxy&) = delete;
	mouse_proxy& operator=(const mouse_proxy&) = delete;

	/**
	 * @brief Mouse button event signal.
	 * Emitted when mouse button event reaches this widget.
	 */
	std::function<bool (widget& widget, bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId)> mouse_button_handler;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId)override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	std::function<bool (widget& widget, const morda::Vec2r& pos, unsigned pointerId)> mouse_move_handler;

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerId)override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	std::function<void(widget& widget, unsigned pointerID)> hover_changed_handler;

	void on_hover_changed(unsigned pointerID)override;
};

}
