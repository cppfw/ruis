#pragma once

#include "ToggleButton.hpp"
#include "../label/Image.hpp"

#include "../../resources/ResImage.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		private Image
{
	std::shared_ptr<ResImage> uncheckedImage;
	std::shared_ptr<ResImage> checkedImage;
	
	void onCheckedChanged() override;
public:
	ImageToggle(const stob::Node* chain = nullptr);
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
