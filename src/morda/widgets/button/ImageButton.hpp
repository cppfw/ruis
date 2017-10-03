#pragma once

#include "Button.hpp"

#include "../label/Image.hpp"

#include "../../resources/ResImage.hpp"

namespace morda{
class ImageButton :
		virtual public Button,
		public Image
{
	std::shared_ptr<const ResImage> unpressedImage;
	std::shared_ptr<const ResImage> pressedImage;
	
protected:
	void onPressedChanged() override;

	ImageButton(const stob::Node* chain);
};	
}
