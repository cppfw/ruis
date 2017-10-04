#pragma once

#include "ToggleButton.hpp"
#include "ImageButton.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		public ImageButton
{
public:
	ImageToggle(const stob::Node* chain = nullptr);
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
