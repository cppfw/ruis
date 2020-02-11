#include "ImagePushButton.hpp"

using namespace morda;

void ImagePushButton::onPressedChanged() {
	this->ImageButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}


ImagePushButton::ImagePushButton(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		PushButton(c, desc),
		ImageButton(c, desc)
{}

