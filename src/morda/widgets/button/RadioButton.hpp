#pragma once

#include "ChoiceButton.hpp"
#include "../NinePatch.hpp"

namespace morda{


class RadioButton :
		public ChoiceButton,
		private NinePatch
{
	std::shared_ptr<Widget> checkWidget;
public:
	RadioButton(const stob::Node* chain = nullptr);
	
	RadioButton(const RadioButton&) = delete;
	RadioButton& operator=(const RadioButton&) = delete;
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override{
		return this->ChoiceButton::onMouseButton(isDown, pos, button, pointerId);
	}
	

	void onHoverChanged(unsigned pointerID)override{
		this->ChoiceButton::onHoverChanged(pointerID);
	}
	
	void onCheckedChanged() override;

private:

};

}
