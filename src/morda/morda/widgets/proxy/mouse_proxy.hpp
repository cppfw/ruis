#pragma once

#include "../widget.hpp"

namespace morda{

/**
 * @brief Widget for intercepting mouse events.
 * From GUI script it can be instantiated as "mouse_proxy".
 */
class mouse_proxy : virtual public widget{
public:
	mouse_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	mouse_proxy(const mouse_proxy&) = delete;
	mouse_proxy& operator=(const mouse_proxy&) = delete;

	/**
	 * @brief Mouse button event signal.
	 * Emitted when mouse button event reaches this widget.
	 */
	std::function<bool(mouse_proxy& w, const mouse_button_event&)> mouse_button_handler;

	bool on_mouse_button(const mouse_button_event&)override;

	/**
	 * @brief Mouse move event signal.
	 * Emitted when mouse move event reaches this widget.
	 */
	std::function<bool(mouse_proxy& w, const mouse_move_event&)> mouse_move_handler;

	bool on_mouse_move(const mouse_move_event& event)override;

	/**
	 * @brief Hover changed event signal.
	 * Emitted when hover changed event reaches this widget.
	 */
	std::function<void(mouse_proxy& w, unsigned pointer_id)> hover_change_handler;

	void on_hover_change(unsigned pointer_id)override;
};

}
