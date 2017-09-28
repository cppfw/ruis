#include "../../Morda.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		ToggleButton(chain),
		Image(nullptr)
{
	if(auto look = getProperty(chain, "look")){
		if(auto p = getProperty(look, "checked")){
			this->pressedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
		if(auto p = getProperty(look, "unchecked")){
			this->unpressedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
	}
	this->onPressedChanged();
}

void ImageToggle::onPressedChanged() {
	if(this->isPressed()){
		this->setImage(this->pressedImage);
	}else{
		this->setImage(this->unpressedImage);
	}
	this->ToggleButton::onPressedChanged();
}
