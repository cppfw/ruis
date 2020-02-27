#include "NinePatchToggle.hpp"


using namespace morda;


void NinePatchToggle::on_pressed_changed(){
	this->NinePatchButton::on_pressed_changed();
	this->ToggleButton::on_pressed_changed();
}


NinePatchToggle::NinePatchToggle(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		ToggleButton(this->context, desc),
		NinePatchButton(this->context, desc)
{}
