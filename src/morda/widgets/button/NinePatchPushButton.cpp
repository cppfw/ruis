#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		PushButton(chain),
		NinePatchButton(chain)
{
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_normal"));
	}
	
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_pressed"));
	}
	
	this->onPressedChanged();
}

void NinePatchPushButton::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}
