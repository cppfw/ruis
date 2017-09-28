#pragma once

#include <functional>

#include "../core/Widget.hpp"



namespace morda{



/**
 * @brief Basic button widget class.
 * Button has two states: normal and pressed.
 */
class Button : public virtual Widget{
	bool isPressed_v = false;
	
protected:
	/**
	 * @brief Called when button pressed state changes.
	 * Default implementation emits the 'pressedChanged' signal.
	 */
	virtual void onPressedChanged();
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) override;
	
	void onHoverChanged(unsigned pointerID) override;
public:	
	Button(const stob::Node* chain) :
			Widget(chain)
	{}
	
	/**
	 * @brief Check if button is currently pressed.
	 * @return true if button is pressed.
	 * @return false otherwise.
	 */
	bool isPressed()const noexcept{
		return this->isPressed_v;
	}
	
	/**
	 * @brief Pressed changed signal.
	 * Emitted when button pressed state changes.
	 */
	std::function<void(Button&)> pressedChanged;
};


}
