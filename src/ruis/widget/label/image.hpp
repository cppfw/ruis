/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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
#include "../base/blending_widget.hpp"
#include "../base/color_widget.hpp"
#include "../widget.hpp"

namespace ruis {

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
	public virtual widget, //
	public blending_widget, //
	public color_widget
{
public:
	/**
	 * @brief Image widget specific parameters.
	 */
	struct specific_parameters {
		/**
		 * @brief Image resource.
		 */
		std::shared_ptr<const ruis::res::image> source;

		/**
		 * @brief Try to keep aspect ratio of the image when scaling during
		 * laying out and measuring.
		 */
		bool keep_aspect_ratio = false;
	};

	/**
	 * @brief Image widget parameters.
	 */
	struct parameters {
		/**
		 * @brief Blending parameters.
		 */
		blending_widget::parameters blending;

		/**
		 * @brief Image color.
		 */
		styled<ruis::color> color;

		/**
		 * @brief Image widget specific parameters.
		 */
		specific_parameters specific;
	};

private:
	specific_parameters params;

	mutable std::shared_ptr<const render::texture_2d> texture;
	mutable utki::shared_ref<const render::vertex_array> vao;

public:
	/**
	 * @brief All image widget parameters.
	 */
	struct all_parameters {
		/**
		 * @brief Layout parameters.
		 */
		ruis::layout_parameters layout_params;

		/**
		 * @brief Base widget parameters.
		 */
		ruis::widget::parameters widget;

		/**
		 * @brief Image widget parameters.
		 */
		image::parameters params;
	};

	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - image widget parameters.
	 */
	image(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	image(const image&) = delete;
	image& operator=(const image&) = delete;

	image(image&&) = delete;
	image& operator=(image&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	~image() override = default;

	/**
	 * @brief Render the image.
	 * @param matrix - transformation matrix.
	 */
	void render(const ruis::mat4& matrix) const override;

	/**
	 * @brief Measure how big the widget wants to be.
	 * @param quotum - space available to the widget.
	 * @param mode - measurement mode for each dimension.
	 * @return Measured desired widget dimensions.
	 */
	ruis::vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const override;

	/**
	 * @brief Set the image resource.
	 * @param image - image resource to display.
	 */
	void set_image(std::shared_ptr<const res::image> image);

	/**
	 * @brief Get the image resource.
	 * @return the image resource currently displayed.
	 */
	const std::shared_ptr<const res::image>& get_image() const
	{
		return this->params.source;
	}

	/**
	 * @brief Invoked when the widget has been resized.
	 */
	void on_resize() override;

	/**
	 * @brief Set keep aspect ratio flag.
	 * @param keep_aspect_ratio - true to keep aspect ratio of the image when scaling.
	 */
	void set_keep_aspect_ratio(bool keep_aspect_ratio)
	{
		this->params.keep_aspect_ratio = keep_aspect_ratio;
		this->invalidate_layout();
	}
};

namespace make {
/**
 * @brief Construct an image widget.
 * @param context - ruis context.
 * @param params - image widget parameters.
 * @return newly constructed image widget.
 */
inline utki::shared_ref<ruis::image> image(
	const utki::shared_ref<ruis::context>& context, //
	image::all_parameters params
)
{
	return utki::make_shared<ruis::image>(
		context, //
		std::move(params)
	);
}

} // namespace make

} // namespace ruis
