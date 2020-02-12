#include "NinePatchToggle.hpp"


using namespace morda;


void NinePatchToggle::onPressedChanged() {
	this->NinePatchButton::onPressedChanged();
	this->ToggleButton::onPressedChanged();
}


NinePatchToggle::NinePatchToggle(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc),
		ToggleButton(this->context, desc),
		NinePatchButton(this->context, desc)
{}
