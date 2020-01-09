#pragma once

#include "ImageButton.hpp"
#include "PushButton.hpp"

namespace morda{
class ImagePushButton :
		public PushButton,
		public ImageButton
{
protected:
	void onPressedChanged() override;

public:
	ImagePushButton(const puu::forest& desc);
};
}
