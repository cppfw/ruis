#pragma once

#include "Button.hpp"
#include "../NinePatch.hpp"

namespace morda{

/**
 * @brief Simple push button.
 * Implementation of a push button as a nine patch which can have children.
 * In GUI scripts can be instantiated as "SimpleButton". Only available after
 * initializing standard morda widgets.
 * Since the button is a NinePatch it can have children.
 */
class SimpleButton :
		public PushButton,
		public NinePatch
{
	std::shared_ptr<ResNinePatch> imgNormal;
	std::shared_ptr<ResNinePatch> imgPressed;
	
public:

	SimpleButton(const SimpleButton&) = delete;
	SimpleButton& operator=(const SimpleButton&) = delete;
	
	SimpleButton(const stob::Node* chain = nullptr);
	
private:
	void onPressedChanged()override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override{
		return this->PushButton::onMouseButton(isDown, pos, button, pointerId);
	}

	void onHoverChanged(unsigned pointerID)override{
		this->PushButton::onHoverChanged(pointerID);
	}
};

}
