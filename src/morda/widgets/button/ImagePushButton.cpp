#include "ImagePushButton.hpp"

using namespace morda;

void ImagePushButton::onPressedChanged() {
	this->ImageButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}


ImagePushButton::ImagePushButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		PushButton(chain),
		ImageButton(chain)
{

}

