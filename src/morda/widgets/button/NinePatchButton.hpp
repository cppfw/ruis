/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "Button.hpp"
#include "../NinePatch.hpp"

namespace morda{

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
	
private:
	void onPressedChanged()override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override{
		return this->PushButton::onMouseButton(isDown, pos, button, pointerId);
	}

	void onHoverChanged(unsigned pointerID)override{
		this->PushButton::onHoverChanged(pointerID);
	}
};

}
