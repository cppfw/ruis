#pragma once

#include "ToggleButton.hpp"
#include "image_button.hpp"

namespace morda{
class image_toggle :
		public ToggleButton,
		public image_button
{
public:
	image_toggle(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	image_toggle(const image_toggle&) = delete;
	image_toggle& operator=(const image_toggle&) = delete;
};
}
