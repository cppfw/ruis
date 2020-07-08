#pragma once

#include "button.hpp"
#include "toggle_button.hpp"

namespace morda{

/**
 * @brief Basic class of a choice button.
 * Choice button is a button which can be checked and belongs to "choice group" of
 * similar buttons and only one button from that group can be checked.
 * When some unchecked button is checked by the user the previously checked button
 * from the same choice group will be unchecked automatically. Well known example
 * of such button is a radio button.
 */
class choice_button : virtual public toggle_button{
	
protected:
	void on_pressed_changed()override;
	
	bool on_mouse_button(const mouse_button_event& event)override;

public:
	choice_button(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	choice_button(const choice_button&) = delete;
	choice_button& operator=(const choice_button&) = delete;
};

}
