/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include "../../res/image.hpp"
#include "../../widget.hpp"
#include "../base/blending_widget.hpp"

namespace morda {

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
class image : public virtual widget, public blending_widget
{
	std::shared_ptr<const morda::res::image> img;
	std::shared_ptr<const morda::res::image> disabled_img; // image for disabled state

	mutable std::shared_ptr<const morda::res::image::texture> texture;

	bool keep_aspect_ratio = false;

	r4::vector2<bool> repeat_v = r4::vector2<bool>(false);

	mutable utki::shared_ref<const vertex_array> vao;

public:
	image(const image&) = delete;
	image& operator=(const image&) = delete;

	image(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

public:
	~image() override = default;

	void render(const morda::matrix4& matrix) const override;

	morda::vector2 measure(const morda::vector2& quotum) const override;

	void set_image(std::shared_ptr<const res::image> image);

	const std::shared_ptr<const res::image>& get_image() const
	{
		return this->img;
	}

	void set_disabled_image(std::shared_ptr<const res::image> image);

	const std::shared_ptr<const res::image>& get_disabled_image() const
	{
		return this->disabled_img;
	}

	void on_resize() override;

	const decltype(repeat_v)& repeat() const noexcept
	{
		return this->repeat_v;
	}

	void set_repeat(decltype(repeat_v) r)
	{
		this->repeat_v = r;
		this->texture.reset();
	}

	void set_keep_aspect_ratio(bool keep_aspect_ratio)
	{
		this->keep_aspect_ratio = keep_aspect_ratio;
		this->invalidate_layout();
	}

	void on_enable_change() override;
};

} // namespace morda
