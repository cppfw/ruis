#include "nine_patch_push_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

nine_patch_push_button::nine_patch_push_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		push_button(this->context, desc),
		nine_patch_button(this->context, desc)
{
	if(!this->get_unpressed_nine_patch()){
		this->set_unpressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_button_normal"));
	}
	
	if(!this->get_pressed_nine_patch()){
		this->set_pressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_button_pressed"));
	}
	
	this->on_pressed_changed();
}

void nine_patch_push_button::on_pressed_changed(){
	this->nine_patch_button::on_pressed_changed();
	this->push_button::on_pressed_changed();
}
