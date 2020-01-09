#pragma once

#include "ToggleButton.hpp"
#include "ImageButton.hpp"

namespace morda{
class ImageToggle :
		public ToggleButton,
		public ImageButton
{
public:
	ImageToggle(const puu::forest& desc);
	ImageToggle(const stob::Node* chain) : ImageToggle(stob_to_puu(chain)){}
	
	ImageToggle(const ImageToggle&) = delete;
	ImageToggle& operator=(const ImageToggle&) = delete;
};
}
