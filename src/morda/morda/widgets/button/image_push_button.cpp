#include "image_push_button.hpp"

using namespace morda;

void image_push_button::on_press_change(){
	this->image_button::on_press_change();
	this->push_button::on_press_change();
}

image_push_button::image_push_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		push_button(this->context, desc),
		image_button(this->context, desc)
{}
