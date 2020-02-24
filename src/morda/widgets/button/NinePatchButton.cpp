#include "NinePatchButton.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc),
		NinePatch(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(pp.value == "unpressed"){
					this->setUnpressedNinePatch(this->context->loader.load<res_ninepatch>(get_property_value(pp).to_string()));
				}else if(pp.value == "pressed"){
					this->setPressedNinePatch(this->context->loader.load<res_ninepatch>(get_property_value(pp).to_string()));
				}
			}
		}
	}
	this->onPressedChanged();
}

void NinePatchButton::onPressedChanged(){
	this->setNinePatch(this->isPressed() ? this->pressedNinePatch_v : this->unpressedNinePatch_v);
	this->Button::onPressedChanged();
}

void NinePatchButton::setPressedNinePatch(std::shared_ptr<const res_ninepatch> np) {
	this->pressedNinePatch_v = std::move(np);
}

void NinePatchButton::setUnpressedNinePatch(std::shared_ptr<const res_ninepatch> np) {
	this->unpressedNinePatch_v = std::move(np);
}
