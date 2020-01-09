#include "ImagePushButton.hpp"

using namespace morda;

void ImagePushButton::onPressedChanged() {
	this->ImageButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}


ImagePushButton::ImagePushButton(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		PushButton(desc),
		ImageButton(desc)
{}

