#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc),
		PushButton(this->context, desc),
		NinePatchButton(this->context, desc)
{
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_button_normal"));
	}
	
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_button_pressed"));
	}
	
	this->onPressedChanged();
}

void NinePatchPushButton::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}
