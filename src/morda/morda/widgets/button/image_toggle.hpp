#pragma once

#include "toggle_button.hpp"
#include "image_button.hpp"

namespace morda{

class image_toggle :
		virtual public toggle_button,
		public image_button
{
public:
	image_toggle(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	image_toggle(const image_toggle&) = delete;
	image_toggle& operator=(const image_toggle&) = delete;
};

}
