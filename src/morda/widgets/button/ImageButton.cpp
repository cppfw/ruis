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

ImageButton::ImageButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		Image(chain)
{
	{
		auto look = getProperty(chain, "look");
		
		if(auto p = getProperty(look, "pressed")){
			this->pressedImage_v = morda::inst().resMan.load<ResImage>(p->value());
		}
		if(auto p = getProperty(look, "unpressed")){
			this->unpressedImage_v = morda::inst().resMan.load<ResImage>(p->value());
		}
		this->updateImage();
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


