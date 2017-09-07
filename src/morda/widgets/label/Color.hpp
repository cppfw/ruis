#pragma once

#include "../base/ColorWidget.hpp"

#include "../../resources/ResGradient.hpp"

namespace morda{


/**
 * @brief Color/gradient widget.
 * This is a widget which can display a rectangle of a single color or with gradient.
 * From GUI script it can be instantiated as "Color".
 * 
 * @param gradient - refer to a gradient resource.
 * 
 */
class Color : public ColorWidget{
	
	std::shared_ptr<ResGradient> gradient;
public:
	Color(const stob::Node* chain = nullptr);
	
	Color(const Color&) = delete;
	Color& operator=(const Color&) = delete;
	
	void render(const morda::Matr4r& matrix)const override;
};



}
