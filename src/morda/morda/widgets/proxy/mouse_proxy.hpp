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
	std::function<bool(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id)> mouse_button_handler;

	bool on_mouse_button(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id)override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	std::function<bool(const morda::vector2& pos, unsigned pointer_id)> mouse_move_handler;

	bool on_mouse_move(const morda::vector2& pos, unsigned pointer_id)override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	std::function<void(bool is_hovered, unsigned pointer_id)> hover_change_handler;

	void on_hover_changed(unsigned pointer_id)override;
};

}
