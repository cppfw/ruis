#include "NinePatchToggle.hpp"


using namespace morda;


void NinePatchToggle::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->ToggleButton::onPressedChanged();
}


NinePatchToggle::NinePatchToggle(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		ToggleButton(chain),
		NinePatchButton(chain)
{

}
