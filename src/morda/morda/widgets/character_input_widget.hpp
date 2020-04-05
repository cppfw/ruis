#pragma once

#include "widget.hpp"

#include "../util/key.hpp"

namespace morda{

/**
 * @brief Base class for widgets which want to get character input events.
 * When the widget has keyboard focus it will also get character input events.
 */
class character_input_widget : virtual public widget{

protected:
	character_input_widget(std::shared_ptr<morda::context> c) :
			widget(std::move(c), puu::forest())
	{}

public:
	/**
	 * @brief Handler for character input.
     * @param unicode - unicode string of entered characters in UTF-32.
	 * @param key - key code associated with character input, can be UNKNOWN.
     */
	virtual void on_character_input(const std::u32string& unicode, key key) = 0;

	virtual ~character_input_widget()noexcept{}
};


}
