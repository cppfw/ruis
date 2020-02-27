#pragma once

#include "choice_button.hpp"
#include "../group/Pile.hpp"

namespace morda{

/**
 * @brief Radio button widget.
 * Radio button is a choice button which displays a thick dot when checked and
 * an empty round when unchecked. In GUI script it can be instantiated as
 * "radio_button". Only available after initializing standard morda widgets.
 */
class radio_button :
		public choice_button,
		private Pile
{
	std::shared_ptr<widget> checkWidget;
public:
	radio_button(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	radio_button(const radio_button&) = delete;
	radio_button& operator=(const radio_button&) = delete;
	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId)override{
		return this->choice_button::on_mouse_button(isDown, pos, button, pointerId);
	}

	void on_hover_changed(unsigned pointerID)override{
		this->choice_button::on_hover_changed(pointerID);
	}
	
	void on_pressed_changed()override;

private:

};

}
