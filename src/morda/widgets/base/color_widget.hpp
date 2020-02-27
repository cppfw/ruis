#pragma once

#include "../widget.hpp"

namespace morda{

/**
 * @brief Basic widget which has a color setting.
 *
 * @param color - color value.
 */
class color_widget : public virtual widget{
	std::uint32_t color_v = 0xffffffff;

protected:
	color_widget(std::shared_ptr<morda::context> c, const puu::forest& desc);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	void set_color(std::uint32_t color){
		if(this->color_v == color){
			return;
		}

		this->color_v = color;
		this->clear_cache();
	}

	std::uint32_t get_color()const noexcept{
		return this->color_v;
	}
};

}
