#pragma once

#include "Button.hpp"


namespace morda{

/**
 * @brief Basic class of a toggle button.
 * Toggle button is a button which can be checked and unchecked.
 * In GUI script it has the 'checked{true/false}' attribute which can set initial
 * checked state of the widget.
 */
class ToggleButton : virtual public Button{
protected:
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) override;
	
public:
	ToggleButton(const puu::forest& desc);
	
	/**
	 * @brief Change checked state to opposite.
	 */
	void toggle(){
		this->setPressed(!this->isPressed());
	}
};

}
