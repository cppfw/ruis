#pragma once

#include "image_button.hpp"
#include "PushButton.hpp"

namespace morda{
class ImagePushButton :
		public PushButton,
		public image_button
{
protected:
	void on_pressed_changed()override;

public:
	ImagePushButton(std::shared_ptr<morda::context> c, const puu::forest& desc);
};
}
