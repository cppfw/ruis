#pragma once

#include "../util/Sides.hpp"

#include "../resources/ResNinePatch.hpp"

#include "core/container/Pile.hpp"
#include "core/container/Table.hpp"

#include "label/ImageLabel.hpp"

#include "base/BlendingWidget.hpp"


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
		private Table
{
	std::shared_ptr<ResNinePatch> image;
	
	std::shared_ptr<ResNinePatch::ImageMatrix> scaledImage;
	
	Sidesr borders;
	
	std::array<std::array<std::shared_ptr<ImageLabel>, 3>, 3> imageLabelMatrix;
	
	std::shared_ptr<Pile> content_var;
	
public:	
	NinePatch(const NinePatch&) = delete;
	NinePatch& operator=(const NinePatch&) = delete;
	
	NinePatch(const stob::Node* chain = nullptr);
	
	void setNinePatch(const std::shared_ptr<ResNinePatch>& np);
	
	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	Pile& content(){
		return *this->content_var;
	}
	
	void render(const morda::Matr4r& matrix) const override;

	/**
	 * @brief Get current border settings.
	 * Border values are in pixels or min_c.
	 * @return Current borders.
	 */
	decltype(borders) getBorders()const noexcept{
		return this->borders;
	}
	
	/**
	 * @brief Show/hide centr4al part of nine-patch.
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
	
	void onBlendingChanged() override;

	
private:
	void applyImages();
};

}
