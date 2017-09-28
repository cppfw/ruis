#pragma once

#include "PushButton.hpp"
#include "NinePatchButton.hpp"


namespace morda{

class NinePatchPushButton :
		public PushButton,
		public NinePatchButton
{
public:
	NinePatchPushButton(const stob::Node* chain);
	
protected:
	void onPressedChanged() override;
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) override{
		return this->PushButton::onMouseButton(isDown, pos, button, pointerID);
	}

	void onHoverChanged(unsigned pointerID) override{
		this->PushButton::onHoverChanged(pointerID);
	}

};
}
