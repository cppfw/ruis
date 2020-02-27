#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		PushButton(this->context, desc),
		NinePatchButton(this->context, desc)
{
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_button_normal"));
	}
	
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_button_pressed"));
	}
	
	this->on_pressed_changed();
}

void NinePatchPushButton::on_pressed_changed(){
	this->NinePatchButton::on_pressed_changed();
	this->PushButton::on_pressed_changed();
}
