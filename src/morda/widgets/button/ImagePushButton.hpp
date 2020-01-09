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
	ImagePushButton(const stob::Node* chain) : ImagePushButton(stob_to_puu(chain)){}
};
}
