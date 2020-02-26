#pragma once

#include <functional>

#include "../widget.hpp"



namespace morda{



/**
 * @brief Basic button widget class.
 * Button has two states: normal and pressed.
 */
class Button : public virtual widget{
	bool isPressed_v = false;
	bool isPressedChangedNotified = true;

protected:
	/**
	 * @brief Called when button pressed state changes.
	 * Default implementation emits the 'pressedChanged' signal.
	 */
	virtual void onPressedChanged();
public:
	Button(std::shared_ptr<morda::context> c, const puu::forest& desc);

	/**
	 * @brief Check if button is currently pressed.
	 * @return true if button is pressed.
	 * @return false otherwise.
	 */
	bool isPressed()const noexcept{
		return this->isPressed_v;
	}

	void setPressed(bool pressed);

	/**
	 * @brief Pressed changed signal.
	 * Emitted when button pressed state changes.
	 */
	std::function<void(Button&)> pressedChanged;
};


}
