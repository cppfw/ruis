#pragma once

#include "../widget.hpp"

#include "../base/blending_widget.hpp"

#include "../../res/res_image.hpp"


namespace morda{

/**
 * @brief Image widget.
 * This widget can display an image.
 * From GUI script it can be instantiated as "Image".
 *
 * @param image - image resource.
 * @param keepAspectRatio - try to keep aspect ratio of the image when scaling.
 * @param repeatX - replicate image horizontally if size of the widget is bigger than size of the image resource.
 * @param repeatY - replicate image vertically if size of the widget is bigger than size of the image resource.
 */
class Image :
		public virtual widget,
		public blending_widget
{
	Image(const Image&);
	Image& operator=(const Image&);

	std::shared_ptr<const morda::res_image> img;

	mutable std::shared_ptr<const morda::res_image::texture> scaledImage;

	bool keepAspectRatio = false;

	r4::vec2b repeat_v = r4::vec2b(false);
	
	mutable std::shared_ptr<vertex_array> vao;

public:
	Image(std::shared_ptr<morda::context> c, const puu::forest& desc);
public:
	virtual ~Image()noexcept{}

	void render(const morda::Matr4r& matrix)const override;

	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	void setImage(const std::shared_ptr<const res_image>& image);

	void on_resize() override;

	const decltype(repeat_v)& repeat()const noexcept{
		return this->repeat_v;
	}

	void setRepeat(decltype(repeat_v) r){
		this->repeat_v = r;
		this->scaledImage.reset();
	}

	void setKeepAspectRatio(bool keepAspectRatio){
		this->keepAspectRatio = keepAspectRatio;
		this->invalidate_layout();
	}
private:

};

}
