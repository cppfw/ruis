#pragma once

#include "../base/fraction_widget.hpp"

namespace morda{

class FractionBandWidget : public fraction_widget{
	float curBandSizeFraction = 0; // Current bar size factor from 0 to 1
protected:
	FractionBandWidget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			fraction_widget(this->context, desc)
	{}

	virtual void onBandSizeChanged();

public:
	std::function<void(FractionBandWidget& s)> band_change_handler;

	void setBandSizeFraction(real fraction);

	real bandSizeFraction()const noexcept{
		return this->curBandSizeFraction;
	}
};

}
