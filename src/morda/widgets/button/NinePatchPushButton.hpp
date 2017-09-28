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
	void onPressedChanged() override{
		this->Button::onPressedChanged();
		this->NinePatchButton::onPressedChanged();
		this->PushButton::onPressedChanged();
	}
};
}
