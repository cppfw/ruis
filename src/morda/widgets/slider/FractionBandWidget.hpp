#pragma once

#include "../base/FractionWidget.hpp"

namespace morda{

class FractionBandWidget : public FractionWidget
{
	float curBandSizeFraction = 0; //Current bar size factor from 0 to 1
protected:
	FractionBandWidget(const stob::Node* chain) :
			Widget(chain),
			FractionWidget(chain)
	{}

	virtual void onBandSizeChanged();

public:
	std::function<void(FractionBandWidget& s)> bandSizeChanged;

	void setBandSizeFraction(real fraction);

	real bandSizeFraction()const noexcept{
		return this->curBandSizeFraction;
	}
};

}
