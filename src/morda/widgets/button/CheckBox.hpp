/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Button.hpp"
#include "../NinePatch.hpp"


namespace morda{


class CheckBox :
		public ToggleButton,
		private NinePatch
{
	std::shared_ptr<Widget> checkWidget;
public:
	CheckBox(const stob::Node* chain = nullptr);
	
	CheckBox(const CheckBox&) = delete;
	CheckBox& operator=(const CheckBox&) = delete;
	
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override{
		return this->ToggleButton::onMouseButton(isDown, pos, button, pointerId);
	}
	

	void onHoverChanged(unsigned pointerID)override{
		this->ToggleButton::onHoverChanged(pointerID);
	}


	void onCheckedChanged()override;

private:

};

}
