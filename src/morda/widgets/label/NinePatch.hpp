#pragma once

#include "../../res/ResNinePatch.hpp"

#include "../group/Pile.hpp"
#include "../group/Column.hpp"

#include "Image.hpp"

#include "../base/BlendingWidget.hpp"


namespace morda{

/**
 * @brief Nine patch widget.
 * Nine patch widget displays a nine-patch and can hold child widgets in its central area.
 * From GUI script it can be instantiated as "NinePatch".
 *
 * @param left - width of left border, in length units.
 * @param right - width of right border, in length units.
 * @param top - height of top border, in length units.
 * @param bottom - height of bottom border, in length units.
 * @param centerVisible - whether the central part of nine-patch image is visible (true) or not (false).
 * @param image - reference to a nine-patch resource.
 */
class NinePatch :
		public virtual Widget,
		public BlendingWidget,
		private Column
{
	std::shared_ptr<const ResNinePatch> image;

	std::shared_ptr<ResNinePatch::ImageMatrix> scaledImage;

	Sidesr borders = Sidesr(layout_params::min);

	std::array<std::array<std::shared_ptr<Image>, 3>, 3> imageMatrix_v;

	std::shared_ptr<Pile> content_v;

public:
	NinePatch(const NinePatch&) = delete;
	NinePatch& operator=(const NinePatch&) = delete;

	NinePatch(const puu::forest& desc);
	NinePatch(const stob::Node* chain) : NinePatch(stob_to_puu(chain)){}

	void setNinePatch(std::shared_ptr<const ResNinePatch> np);

	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	Pile& content(){
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

	void onBlendingChanged() override;


private:
	void applyImages();
};

}
