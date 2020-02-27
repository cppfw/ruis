#pragma once

#include "button.hpp"

#include "../label/Image.hpp"

#include "../../res/res_image.hpp"

namespace morda{
class ImageButton :
		virtual public button,
		public Image
{
	std::shared_ptr<const res_image> unpressedImage_v;
	std::shared_ptr<const res_image> pressedImage_v;
	
protected:
	void on_pressed_changed()override;

	ImageButton(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	void updateImage();
	
public:
	void setPressedImage(std::shared_ptr<const res_image> image);
	
	const decltype(pressedImage_v)& pressedImage()const{
		return this->pressedImage_v;
	}
	
	void setUnpressedImage(std::shared_ptr<const res_image> image);
	
	const decltype(unpressedImage_v)& unpressedImage()const{
		return this->unpressedImage_v;
	}
};	
}
