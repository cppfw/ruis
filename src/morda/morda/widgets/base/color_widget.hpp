#pragma once

#include "../widget.hpp"

namespace morda{

/**
 * @brief Basic widget which has a color setting.
 *
 * @param color - color value.
 * @param disabled_color - color value for disabled state.
 */
class color_widget : public virtual widget{
	uint32_t color = 0xffffffff;
	uint32_t disabled_color = 0xff808080;
protected:
	color_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	void set_color(uint32_t color);

	uint32_t get_color()const noexcept{
		return this->color;
	}

	void set_disabled_color(uint32_t color);

	uint32_t get_disabled_color()const noexcept{
		return this->disabled_color;
	}

	/**
	 * @brief Get color for current enabled/disabled state.
	 * @return color for the current enabled/disabled state.
	 */
	uint32_t get_current_color()const noexcept;
};

}
