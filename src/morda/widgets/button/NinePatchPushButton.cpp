#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		PushButton(c, desc),
		NinePatchButton(c, desc)
{
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(morda::gui::inst().context->loader.load<ResNinePatch>("morda_npt_button_normal"));
	}
	
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(morda::gui::inst().context->loader.load<ResNinePatch>("morda_npt_button_pressed"));
	}
	
	this->onPressedChanged();
}

void NinePatchPushButton::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}
