#pragma once

#include "../../res/res_nine_patch.hpp"

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
 * @param centerVisible - whether the central part of nine-patch image is visible (true) or not (false).
 * @param image - reference to a nine-patch resource.
 */
class nine_patch :
		public virtual widget,
		public blending_widget,
		private column
{
	std::shared_ptr<const res_nine_patch> res;

	std::shared_ptr<res_nine_patch::image_matrix> scaledImage;

	Sidesr borders = Sidesr(layout_params::min);

	std::array<std::array<std::shared_ptr<image>, 3>, 3> imageMatrix_v;

	std::shared_ptr<pile> content_v;

public:
	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;

	nine_patch(std::shared_ptr<morda::context> c, const puu::forest& desc);

	void setNinePatch(std::shared_ptr<const res_nine_patch> np);

	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	pile& content(){
		return *this->content_v;
	}

	void render(const morda::Matr4r& matrix) const override;

	/**
	 * @brief Show/hide central part of nine-patch.
	 * @param visible - show (true) or hide (false) central part of the nine-patch.
	 */
	void setCenterVisible(bool visible);

	/**
	 * @brief Set border settings.
	 * Border values are in pixels or min_c.
	 * @param borders - border values to set.
	 */
	void setBorders(Sidesr borders){
		this->borders = borders;
		this->applyImages();
	}

	/**
	 * @brief Get current border settings.
	 * Border values are in pixels or min_c.
	 * @return Current borders.
	 */
	decltype(borders) getBorders()const noexcept{
		return this->borders;
	}

	Sidesr getActualBorders()const noexcept;

	void on_blending_changed()override;


private:
	void applyImages();
};

}
