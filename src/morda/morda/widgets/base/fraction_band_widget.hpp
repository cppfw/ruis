#pragma once

#include "../base/fraction_widget.hpp"

namespace morda{

class fraction_band_widget : public fraction_widget{
	float curBandSizeFraction = 0; // Current bar size factor from 0 to 1
protected:
	fraction_band_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			fraction_widget(this->context, desc)
	{}

	virtual void on_band_change();

public:
	std::function<void(fraction_band_widget& s)> band_change_handler;

	void set_band_fraction(real fraction);

	real get_band_fraction()const noexcept{
		return this->curBandSizeFraction;
	}
};

}
