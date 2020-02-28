#pragma once

#include "../widget.hpp"

#include "../base/blending_widget.hpp"

#include "../../res/res_image.hpp"

namespace morda{

/**
 * @brief Image widget.
 * This widget can display an image.
 * From GUI script it can be instantiated as "image".
 *
 * @param image - image resource.
 * @param keep_aspect_ratio - try to keep aspect ratio of the image when scaling.
 * @param repeat_x - replicate image horizontally if size of the widget is bigger than size of the image resource.
 * @param repeat_y - replicate image vertically if size of the widget is bigger than size of the image resource.
 */
class image :
		public virtual widget,
		public blending_widget
{
	image(const image&);
	image& operator=(const image&);

	std::shared_ptr<const morda::res_image> img;

	mutable std::shared_ptr<const morda::res_image::texture> scaledImage;

	bool keep_aspect_ratio = false;

	r4::vec2b repeat_v = r4::vec2b(false);
	
	mutable std::shared_ptr<vertex_array> vao;

public:
	image(std::shared_ptr<morda::context> c, const puu::forest& desc);
public:
	virtual ~image()noexcept{}

	void render(const morda::Matr4r& matrix)const override;

	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	void set_image(const std::shared_ptr<const res_image>& image);

	void on_resize() override;

	const decltype(repeat_v)& repeat()const noexcept{
		return this->repeat_v;
	}

	void set_repeat(decltype(repeat_v) r){
		this->repeat_v = r;
		this->scaledImage.reset();
	}

	void set_keep_aspect_ratio(bool keep_aspect_ratio){
		this->keep_aspect_ratio = keep_aspect_ratio;
		this->invalidate_layout();
	}
private:

};

}
