#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		PushButton(desc),
		NinePatchButton(desc)
{
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(morda::context::inst().resMan.load<ResNinePatch>("morda_npt_button_normal"));
	}
	
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(morda::context::inst().resMan.load<ResNinePatch>("morda_npt_button_pressed"));
	}
	
	this->onPressedChanged();
}

void NinePatchPushButton::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}
