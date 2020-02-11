#include "NinePatchToggle.hpp"


using namespace morda;


void NinePatchToggle::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->ToggleButton::onPressedChanged();
}


NinePatchToggle::NinePatchToggle(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		ToggleButton(c, desc),
		NinePatchButton(c, desc)
{}
