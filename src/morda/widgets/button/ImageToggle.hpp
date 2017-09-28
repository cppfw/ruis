#pragma once

#include "ToggleButton.hpp"
#include "../label/Image.hpp"

#include "../../resources/ResImage.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		private Image
{
	std::shared_ptr<ResImage> unpressedImage;
	std::shared_ptr<ResImage> pressedImage;
	
	void onPressedChanged() override;
public:
	ImageToggle(const stob::Node* chain = nullptr);
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
