#pragma once

#include "../../res/nine_patch.hpp"

#include "../group/pile.hpp"
#include "../group/column.hpp"

#include "image.hpp"

#include "../base/blending_widget.hpp"

namespace morda{

/**
 * @brief Nine patch widget.
 * Nine patch widget displays a nine-patch and can hold child widgets in its central area.
 * From GUI script it can be instantiated as "nine_patch".
 *
 * @param left - width of left border, in length units.
 * @param right - width of right border, in length units.
 * @param top - height of top border, in length units.
 * @param bottom - height of bottom border, in length units.
 * @param center_visible - whether the central part of nine-patch image is visible (true) or not (false).
 * @param image - reference to a nine-patch resource.
 */
class nine_patch :
		public virtual widget,
		public blending_widget,
		private column
{
	std::shared_ptr<const res::nine_patch> np_res;

	std::shared_ptr<res::nine_patch::image_matrix> texture;

	sides<real> borders = sides<real>(layout_params::min);

	std::array<std::array<std::shared_ptr<image>, 3>, 3> img_widgets_matrix;

	std::shared_ptr<pile> inner_content;

public:
	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;

	nine_patch(std::shared_ptr<morda::context> c, const puu::forest& desc);

	void set_nine_patch(std::shared_ptr<const res::nine_patch> np);

	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	pile& content(){
		return *this->inner_content;
	}

	void render(const morda::matrix4& matrix) const override;

	/**
	 * @brief Show/hide central part of nine-patch.
	 * @param visible - show (true) or hide (false) central part of the nine-patch.
	 */
	void set_center_visible(bool visible);

	/**
	 * @brief Set border settings.
	 * Border values are in pixels or min_c.
	 * @param borders - border values to set.
	 */
	void set_borders(sides<real> borders){
		this->borders = borders;
		this->applyImages();
	}

	/**
	 * @brief Get current border settings.
	 * Border values are in pixels or min_c.
	 * @return Current borders.
	 */
	decltype(borders) get_borders()const noexcept{
		return this->borders;
	}

	sides<real> get_actual_borders()const noexcept;

	void on_blending_change()override;

private:
	void applyImages();
};

}
