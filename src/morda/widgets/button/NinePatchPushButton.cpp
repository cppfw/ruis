#include "NinePatchPushButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

NinePatchPushButton::NinePatchPushButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		PushButton(chain),
		NinePatchButton(chain)
{
	{
		auto ch = getProperty(chain, "look");
		
		if(auto n = getProperty(ch, "unpressed")){
			this->setUnpressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			this->setUnpressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_normal"));
		}

		if(auto n = getProperty(ch, "pressed")){
			this->setPressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			this->setPressedNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_pressed"));
		}
	}
	
	//initialize nine-patch
	this->NinePatchButton::onPressedChanged();
}

void NinePatchPushButton::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}
