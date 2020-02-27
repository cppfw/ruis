#pragma once

#include "PushButton.hpp"
#include "nine_patch_button.hpp"


namespace morda{

class NinePatchPushButton :
		public PushButton,
		public nine_patch_button
{
public:
	NinePatchPushButton(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
protected:
	void on_pressed_changed()override;
	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID) override{
		return this->PushButton::on_mouse_button(isDown, pos, button, pointerID);
	}

	void on_hover_changed(unsigned pointerID) override{
		this->PushButton::on_hover_changed(pointerID);
	}

};
}
