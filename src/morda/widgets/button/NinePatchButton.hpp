#pragma once

#include "PushButton.hpp"
#include "../NinePatch.hpp"

namespace morda{

/**
 * @brief Simple push button.
 * Implementation of a push button as a nine patch which can have children.
 * In GUI scripts can be instantiated as "SimpleButton". Only available after
 * initializing standard morda widgets.
 * Since the button is a NinePatch it can have children.
 */
class NinePatchButton :
		public PushButton,
		public NinePatch
{
	std::shared_ptr<ResNinePatch> imgNormal;
	std::shared_ptr<ResNinePatch> imgPressed;
	
public:

	NinePatchButton(const NinePatchButton&) = delete;
	NinePatchButton& operator=(const NinePatchButton&) = delete;
	
	NinePatchButton(const stob::Node* chain = nullptr);
	
protected:
	void onPressedChanged()override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override{
		return this->PushButton::onMouseButton(isDown, pos, button, pointerId);
	}

	void onHoverChanged(unsigned pointerID)override{
		this->PushButton::onHoverChanged(pointerID);
	}
};

}
