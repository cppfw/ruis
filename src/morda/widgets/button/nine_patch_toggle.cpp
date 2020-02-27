#include "nine_patch_toggle.hpp"

using namespace morda;

void nine_patch_toggle::on_pressed_changed(){
	this->nine_patch_button::on_pressed_changed();
	this->toggle_button::on_pressed_changed();
}

nine_patch_toggle::nine_patch_toggle(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		nine_patch_button(this->context, desc)
{}
