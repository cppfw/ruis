#include "NinePatchButton.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		NinePatch(chain)
{
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
