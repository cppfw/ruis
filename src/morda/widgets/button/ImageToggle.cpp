#include "../../Morda.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain),
		Image(nullptr)
{
	if(auto look = getProperty(chain, "look")){
		if(auto p = getProperty(look, "checked")){
			this->checkedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
		if(auto p = getProperty(look, "unchecked")){
			this->uncheckedImage = morda::inst().resMan.load<ResImage>(p->value());
		}
	}
	this->onCheckedChanged();
}

void ImageToggle::onCheckedChanged() {
	if(this->isChecked()){
		this->setImage(this->checkedImage);
	}else{
		this->setImage(this->uncheckedImage);
	}
	this->ToggleButton::onCheckedChanged();
}
