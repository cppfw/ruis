#pragma once

#include "image_button.hpp"
#include "push_button.hpp"

namespace morda{
class image_push_button :
		public push_button,
		public image_button
{
protected:
	void on_press_change()override;

public:
	image_push_button(std::shared_ptr<morda::context> c, const puu::forest& desc);
};
}
