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
	CheckBox(const std::shared_ptr<morda::context>& c, const puu::forest& desc);
	
	CheckBox(const CheckBox&) = delete;
	CheckBox& operator=(const CheckBox&) = delete;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override{
		return this->ToggleButton::on_mouse_button(isDown, pos, button, pointerId);
	}

	void on_hover_changed(unsigned pointerID)override{
		this->ToggleButton::on_hover_changed(pointerID);
	}

	void onPressedChanged()override;

private:

};

}
