#pragma once

#include "../widget.hpp"

namespace morda{

/**
 * @brief Widget for intercepting resize events.
 * From GUI script it can be instantiated as "resize_proxy".
 */
class resize_proxy : virtual public widget{
public:
	resize_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc)
	{}

	resize_proxy(const resize_proxy&) = delete;
	resize_proxy& operator=(const resize_proxy&) = delete;

	void on_resize()override;

	/**
	 * @brief Resize event signal.
	 * Emitted when this widget is resized.
	 */
	std::function<void(const vector2&)> resize_handler;
};

}
