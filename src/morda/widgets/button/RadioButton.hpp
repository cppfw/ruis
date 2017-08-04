#pragma once

#include "ChoiceButton.hpp"
#include "../core/container/Pile.hpp"

namespace morda{

/**
 * @brief Radio button widget.
 * Radio button is a choice button which displays a thick dot when checked and
 * an empty round when unchecked. In GUI script it can be instantiated as
 * "RadioButton". Only available after initializing standard morda widgets.
 */
class RadioButton :
		public ChoiceButton,
		private Pile
{
	std::shared_ptr<Widget> checkWidget;
public:
	RadioButton(const stob::Node* chain = nullptr);
	
	RadioButton(const RadioButton&) = delete;
	RadioButton& operator=(const RadioButton&) = delete;
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override{
		return this->ChoiceButton::onMouseButton(isDown, pos, button, pointerId);
	}
	

	void onHoverChanged(unsigned pointerID)override{
		this->ChoiceButton::onHoverChanged(pointerID);
	}
	
	void onCheckedChanged() override;

private:

};

}
