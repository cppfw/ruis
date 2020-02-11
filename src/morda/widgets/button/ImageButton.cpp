#include "ImageButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

void ImageButton::updateImage() {
	if(this->isPressed()){
		this->setImage(this->pressedImage_v);
	}else{
		this->setImage(this->unpressedImage_v);
	}
}

void ImageButton::onPressedChanged() {
	this->updateImage();
	this->Button::onPressedChanged();
}

ImageButton::ImageButton(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		Image(c, desc)
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
					this->pressedImage_v = morda::inst().context->loader.load<ResImage>(get_property_value(pp).to_string());
				}else if(pp.value == "unpressed"){
					this->unpressedImage_v = morda::inst().context->loader.load<ResImage>(get_property_value(pp).to_string());
				}
				this->updateImage();
			}
		}
	}
}

void ImageButton::setPressedImage(std::shared_ptr<const ResImage> image) {
	this->pressedImage_v = std::move(image);
	this->updateImage();
}

void ImageButton::setUnpressedImage(std::shared_ptr<const ResImage> image) {
	this->unpressedImage_v = std::move(image);
	this->updateImage();
}


