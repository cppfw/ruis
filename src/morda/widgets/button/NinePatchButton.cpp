#include "NinePatchButton.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		NinePatch(desc)
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
					this->setUnpressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(get_property_value(pp).to_string()));
				}else if(pp.value == "pressed"){
					this->setPressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(get_property_value(pp).to_string()));
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

void NinePatchButton::setPressedNinePatch(std::shared_ptr<const ResNinePatch> np) {
	this->pressedNinePatch_v = std::move(np);
}

void NinePatchButton::setUnpressedNinePatch(std::shared_ptr<const ResNinePatch> np) {
	this->unpressedNinePatch_v = std::move(np);
}
