#pragma once

#include "../base/ColorWidget.hpp"

#include "../../resources/ResGradient.hpp"

namespace morda{


/**
 * @brief Color/gradient widget.
 * This is a widget which can display a rectangle of a single color or with gradient.
 * From GUI script it can be instantiated as "ColorLabel".
 * 
 * @param gradient - refer to a gradient resource.
 * 
 */
class ColorLabel : public ColorWidget{
	
	std::shared_ptr<ResGradient> gradient;
public:
	ColorLabel(const stob::Node* chain = nullptr);
	
	ColorLabel(const ColorLabel&) = delete;
	ColorLabel& operator=(const ColorLabel&) = delete;
	
	void render(const morda::Matr4r& matrix)const override;
};



}
