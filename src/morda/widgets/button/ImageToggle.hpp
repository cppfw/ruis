#pragma once

#include "ToggleButton.hpp"
#include "image_button.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		public image_button
{
public:
	ImageToggle(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
