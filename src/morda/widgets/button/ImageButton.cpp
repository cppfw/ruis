#include "ImageButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

void ImageButton::onPressedChanged() {
	if(this->isPressed()){
		this->setImage(this->pressedImage_v);
	}else{
		this->setImage(this->unpressedImage_v);
	}
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
	}
	this->onPressedChanged();
}

void ImageButton::setPressedImage(std::shared_ptr<const ResImage> image) {
	this->pressedImage_v = std::move(image);
}

void ImageButton::setUnpressedImage(std::shared_ptr<const ResImage> image) {
	this->unpressedImage_v = std::move(image);
}


