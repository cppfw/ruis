#include "ImagePushButton.hpp"

using namespace morda;

void ImagePushButton::onPressedChanged() {
	this->ImageButton::onPressedChanged();
	this->PushButton::onPressedChanged();
}


ImagePushButton::ImagePushButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc),
		PushButton(this->context, desc),
		ImageButton(this->context, desc)
{}

