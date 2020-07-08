#include "nine_patch_button.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

nine_patch_button::nine_patch_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		nine_patch(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(pp.value == "unpressed"){
					this->set_unpressed_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(pp).to_string()));
				}else if(pp.value == "pressed"){
					this->set_pressed_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(pp).to_string()));
				}
			}
		}
	}
	this->on_press_change();
}

void nine_patch_button::on_press_change(){
	this->set_nine_patch(this->is_pressed() ? this->pressedNinePatch_v : this->unpressedNinePatch_v);
	this->button::on_press_change();
}

void nine_patch_button::set_pressed_nine_patch(std::shared_ptr<const res::nine_patch> np){
	this->pressedNinePatch_v = std::move(np);
}

void nine_patch_button::set_unpressed_nine_patch(std::shared_ptr<const res::nine_patch> np){
	this->unpressedNinePatch_v = std::move(np);
}
