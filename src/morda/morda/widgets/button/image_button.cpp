#include "image_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

void image_button::update_image(){
	if(this->is_pressed()){
		this->set_image(this->pressedImage_v);
	}else{
		this->set_image(this->unpressedImage_v);
	}
}

void image_button::on_press_change(){
	this->update_image();
	this->button::on_press_change();
}

image_button::image_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		image(this->context, desc)
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

				if(pp.value == "pressed"){
					this->pressedImage_v = this->context->loader.load<res::image>(get_property_value(pp).to_string());
				}else if(pp.value == "unpressed"){
					this->unpressedImage_v = this->context->loader.load<res::image>(get_property_value(pp).to_string());
				}
				this->update_image();
			}
		}
	}
}

void image_button::set_pressed_image(std::shared_ptr<const res::image> image){
	this->pressedImage_v = std::move(image);
	this->update_image();
}

void image_button::set_unpressed_image(std::shared_ptr<const res::image> image){
	this->unpressedImage_v = std::move(image);
	this->update_image();
}
