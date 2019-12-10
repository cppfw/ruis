#pragma once

#include "Button.hpp"

namespace morda{

/**
 * @brief Basic class of a push button.
 * Push button is a button which can be clicked.
 * Clicked means that the button was pressed and then released.
 */
class PushButton : virtual public Button{
	bool currentlyPressed = false;

protected:
	void onPressedChanged()override;
	
	/**
	 * @brief Invoked when the button is clicked.
	 * Default implementation emits 'clicked' signal.
	 */
	virtual void onClicked();
	
	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) override;
	
	void on_hover_changed(unsigned pointerID) override;
	
public:
	PushButton(const stob::Node* chain) :
			Widget(chain),
			Button(chain)
	{}
	
	/**
	 * @brief Clicked signal.
	 * Emitted when the button is clicked.
	 */
	std::function<void(PushButton&)> clicked;
};

}
