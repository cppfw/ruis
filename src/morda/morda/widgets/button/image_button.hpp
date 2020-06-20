#pragma once

#include "button.hpp"

#include "../label/image.hpp"

#include "../../res/image.hpp"

namespace morda{
class image_button :
		virtual public button,
		public image
{
	std::shared_ptr<const res::image> unpressedImage_v;
	std::shared_ptr<const res::image> pressedImage_v;
	
	void update_image();
protected:
	void on_pressed_changed()override;

	image_button(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
public:
	void set_pressed_image(std::shared_ptr<const res::image> image);
	
	const decltype(pressedImage_v)& get_pressed_image()const{
		return this->pressedImage_v;
	}
	
	void set_unpressed_image(std::shared_ptr<const res::image> image);
	
	const decltype(unpressedImage_v)& get_unpressed_image()const{
		return this->unpressedImage_v;
	}
};	
}
