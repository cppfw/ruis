#pragma once

#include "ToggleButton.hpp"
#include "../label/NinePatch.hpp"


namespace morda{

/**
 * @brief Checkbox widget.
 * Checkbox is a toggle button which shows tick sign when checked and empty box when unchecked.
 * In GUI script can be instantiated as 'Checkbox'. Only available after initializing
 * standard morda widgets.
 */
class CheckBox :
		public ToggleButton,
		private NinePatch
{
	std::shared_ptr<Widget> checkWidget;
public:
	CheckBox(const stob::Node* chain = nullptr);
	
	CheckBox(const CheckBox&) = delete;
	CheckBox& operator=(const CheckBox&) = delete;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override{
		return this->ToggleButton::onMouseButton(isDown, pos, button, pointerId);
	}

	void onHoverChanged(unsigned pointerID)override{
		this->ToggleButton::onHoverChanged(pointerID);
	}

	void onCheckedChanged()override;

private:

};

}
