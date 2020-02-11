#pragma once

#include "Button.hpp"

#include "../label/Image.hpp"

#include "../../res/ResImage.hpp"

namespace morda{
class ImageButton :
		virtual public Button,
		public Image
{
	std::shared_ptr<const ResImage> unpressedImage_v;
	std::shared_ptr<const ResImage> pressedImage_v;
	
protected:
	void onPressedChanged() override;

	ImageButton(const std::shared_ptr<morda::context>& c, const puu::forest& desc);
	
	void updateImage();
	
public:
	void setPressedImage(std::shared_ptr<const ResImage> image);
	
	const decltype(pressedImage_v)& pressedImage()const{
		return this->pressedImage_v;
	}
	
	void setUnpressedImage(std::shared_ptr<const ResImage> image);
	
	const decltype(unpressedImage_v)& unpressedImage()const{
		return this->unpressedImage_v;
	}
};	
}
