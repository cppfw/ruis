#pragma once

#include "../base/color_widget.hpp"


namespace morda{


/**
 * @brief Color widget.
 * This is a widget which can display a rectangle of a single color.
 * From GUI script it can be instantiated as "color".
 */
class color : public color_widget{
public:
	color(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	color(const color&) = delete;
	color& operator=(const color&) = delete;

	void render(const morda::matrix4& matrix)const override;
};



}
