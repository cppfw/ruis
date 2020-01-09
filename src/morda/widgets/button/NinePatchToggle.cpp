#include "NinePatchToggle.hpp"


using namespace morda;


void NinePatchToggle::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->ToggleButton::onPressedChanged();
}


NinePatchToggle::NinePatchToggle(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		ToggleButton(desc),
		NinePatchButton(desc)
{}
