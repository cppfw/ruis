#pragma once

#include "PushButton.hpp"
#include "NinePatchButton.hpp"


namespace morda{

class NinePatchPushButton :
		public PushButton,
		public NinePatchButton
{
public:
	NinePatchPushButton(const puu::forest& desc);
	NinePatchPushButton(const stob::Node* chain) : NinePatchPushButton(stob_to_puu(chain)){}
	
protected:
	void onPressedChanged() override;
	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) override{
		return this->PushButton::on_mouse_button(isDown, pos, button, pointerID);
	}

	void on_hover_changed(unsigned pointerID) override{
		this->PushButton::on_hover_changed(pointerID);
	}

};
}
