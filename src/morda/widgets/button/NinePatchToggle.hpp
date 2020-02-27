#pragma once

#include "ToggleButton.hpp"
#include "NinePatchButton.hpp"


namespace morda{
class NinePatchToggle :
		virtual public ToggleButton,
		public NinePatchButton
{
public:
	NinePatchToggle(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
protected:
	void on_pressed_changed()override;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID)override{
		return this->ToggleButton::on_mouse_button(isDown, pos, button, pointerID);
	}

};
}
