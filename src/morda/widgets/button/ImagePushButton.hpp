#pragma once

#include "ImageButton.hpp"
#include "PushButton.hpp"

namespace morda{
class ImagePushButton :
		public PushButton,
		public ImageButton
{
protected:
	void on_pressed_changed()override;

public:
	ImagePushButton(std::shared_ptr<morda::context> c, const puu::forest& desc);
};
}
