#include "nine_patch_toggle.hpp"

using namespace morda;

void nine_patch_toggle::on_press_change(){
	this->nine_patch_button::on_press_change();
	this->toggle_button::on_press_change();
}

nine_patch_toggle::nine_patch_toggle(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		nine_patch_button(this->context, desc)
{}
