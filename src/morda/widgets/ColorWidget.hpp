/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "core/Widget.hpp"


namespace morda{



class ColorWidget : public virtual Widget{
	std::uint32_t color_var;
	
protected:
	ColorWidget(const stob::Node* chain);
	
public:
	ColorWidget(const ColorWidget&) = delete;
	ColorWidget& operator=(const ColorWidget&) = delete;
	
	
	void setColor(std::uint32_t color){
		if(this->color_var == color){
			return;
		}
		
		this->color_var = color;
		this->clearCache();
	}
	
	std::uint32_t color()const noexcept{
		return this->color_var;
	}
};


}
