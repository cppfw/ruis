#pragma once

#include "../base/color_widget.hpp"


namespace morda{


/**
 * @brief Color widget.
 * This is a widget which can display a rectangle of a single color.
 * From GUI script it can be instantiated as "Color".
 */
class Color : public color_widget{
public:
	Color(std::shared_ptr<morda::context> c, const puu::forest& desc);

	Color(const Color&) = delete;
	Color& operator=(const Color&) = delete;

	void render(const morda::Matr4r& matrix)const override;
};



}
