#pragma once

#include "Button.hpp"
#include "../label/ImageLabel.hpp"

#include "../../resources/ResImage.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		private ImageLabel
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
