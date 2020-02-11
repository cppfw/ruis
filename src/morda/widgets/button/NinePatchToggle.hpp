#pragma once

#include "ToggleButton.hpp"
#include "NinePatchButton.hpp"


namespace morda{
class NinePatchToggle :
		virtual public ToggleButton,
		public NinePatchButton
{
public:
	NinePatchToggle(const std::shared_ptr<morda::context>& c, const puu::forest& desc);
	
protected:
	void onPressedChanged() override;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) override{
		return this->ToggleButton::on_mouse_button(isDown, pos, button, pointerID);
	}

};
}
