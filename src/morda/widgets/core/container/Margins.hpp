#pragma once

#include "../../../config.hpp"

#include "../../../util/Sides.hpp"

#include "LinearArea.hpp"


namespace morda{

class Margins : 
		virtual public Widget,
		private HorizontalArea
{
	std::shared_ptr<Widget> child;
	
	//can hold also Fill_d or Max_d
	Sidesr margins_v;
public:
	Margins(const stob::Node* chain = nullptr);
	
	Margins(const Margins&) = delete;
	Margins& operator=(const Margins&) = delete;
	
	void setWidget(std::shared_ptr<Widget> w);
	
	const Sidesr& margins()const noexcept{
		return this->margins_v;
	}
	
	/**
	 * @brief Set new margins.
	 * Margin can be expressed in pixels (no unit), millimeters (mm) or points (pt).
	 * It can also be 'fill', 'min' or 'max'. 'fill' and 'max' have same effect.
	 * 'min' has same effect as zero margin.
	 * @param margins - margin values to set.
	 */
	void setMargins(Sidesr margins);
};

}
