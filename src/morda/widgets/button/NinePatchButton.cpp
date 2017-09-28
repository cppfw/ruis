#include "NinePatchButton.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		NinePatch(chain)
{
	{
		auto ch = getProperty(chain, "look");
		
		if(auto n = getProperty(ch, "unpressed")){
			this->setUnpressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}

		if(auto n = getProperty(ch, "pressed")){
			this->setPressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}
	}
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
