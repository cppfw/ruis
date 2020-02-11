#pragma once

#include "ToggleButton.hpp"
#include "ImageButton.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		public ImageButton
{
public:
	ImageToggle(const std::shared_ptr<morda::context>& c, const puu::forest& desc);
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
