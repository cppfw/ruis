#pragma once

#include "../../../config.hpp"

#include "../../../util/Sides.hpp"

#include "LinearArea.hpp"


namespace morda{

/**
 * @brief Margins widget.
 * This widget is a container widget which can have only one child and is used for adding margins to some widget.
 * From GUI script it can be instantiated as "Margins".It can have one child widget.
 * It can have properties for setting margin values expressed in pixels (no unit), millimeters (mm) or points (pt):
 * @param left - left margin.
 * @param right - right margin.
 * @param top - top margin.
 * @param bottom - bottom margin.
 * 
 * Example:
 * @code
 * Margins{
 *     left{35}
 *     top{4pt}
 *     right{10mm}
 *     bottom{27}
 * 
 *     TextLabel{
 *         text{"Hello world!"}
 *     }
 * }
 * @endcode
 */
class Margins : 
		virtual public Widget,
		private HorizontalArea
{
	std::shared_ptr<Widget> child;
	
	//can hold also fill_c or max_c
	Sidesr margins_v;
public:
	/**
	 * @brief Constructor.
	 * @param chain - STOB chain describing the widget.
	 */
	Margins(const stob::Node* chain = nullptr);
	
	Margins(const Margins&) = delete;
	Margins& operator=(const Margins&) = delete;
	
	/**
	 * @brief Set child widget.
	 * @param w - widget to set as a child widget.
	 */
	void setWidget(std::shared_ptr<Widget> w);
	
	/**
	 * @brief Get current margin values.
	 * @return Margin values.
	 */
	const Sidesr& margins()const noexcept{
		return this->margins_v;
	}
	
	/**
	 * @brief Set new margins.
	 * MArgins can be values in pixels or can also be 'fill', 'min' or 'max' from Widget::LayoutParams class. 'fill' and 'max' have same effect.
	 * 'min' has same effect as zero margin.
	 * @param margins - margin values to set.
	 */
	void setMargins(Sidesr margins);
};

}
