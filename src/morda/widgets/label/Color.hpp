#pragma once

#include "../base/ColorWidget.hpp"


namespace morda{


/**
 * @brief Color widget.
 * This is a widget which can display a rectangle of a single color.
 * From GUI script it can be instantiated as "Color".
 */
class Color : public ColorWidget{
public:
	Color(const puu::forest& desc);
	Color(const stob::Node* chain) : Color(stob_to_puu(chain)){}

	Color(const Color&) = delete;
	Color& operator=(const Color&) = delete;

	void render(const morda::Matr4r& matrix)const override;
};



}
