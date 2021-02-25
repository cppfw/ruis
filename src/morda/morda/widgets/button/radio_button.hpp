#pragma once

#include "choice_button.hpp"
#include "../group/pile.hpp"

namespace morda{

/**
 * @brief Radio button widget.
 * Radio button is a choice button which displays a thick dot when checked and
 * an empty round when unchecked. In GUI script it can be instantiated as
 * "radio_button". Only available after initializing standard morda widgets.
 */
class radio_button :
		virtual public choice_button,
		private pile
{
	std::shared_ptr<widget> checkWidget;
public:
	radio_button(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	radio_button(const radio_button&) = delete;
	radio_button& operator=(const radio_button&) = delete;
	
	bool on_mouse_button(const mouse_button_event& e)override{
		return this->choice_button::on_mouse_button(e);
	}

	void on_hover_change(unsigned pointer_id)override{
		this->choice_button::on_hover_change(pointer_id);
	}
	
	void on_press_change()override;

private:

};

}
