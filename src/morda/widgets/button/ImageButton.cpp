#include "ImageButton.hpp"

#include "../../util/util.hpp"

using namespace morda;

void ImageButton::onPressedChanged() {
	if(this->isPressed()){
		this->setImage(this->pressedImage);
	}else{
		this->setImage(this->unpressedImage);
	}
	this->Button::onPressedChanged();
}

ImageButton::ImageButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain)
{
	{
		auto look = getProperty(chain, "look");
		
		if(auto p = getProperty(look, "pressed")){
			this->pressedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
		if(auto p = getProperty(look, "unpressed")){
			this->unpressedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
	}
	this->onPressedChanged();
}
