#include "FractionBandWidget.hpp"

using namespace morda;

void FractionBandWidget::onBandSizeChanged() {
	if (this->bandSizeChanged) {
		this->bandSizeChanged(*this);
	}
}

void FractionBandWidget::setBandSizeFraction(real fraction) {
	if (this->curBandSizeFraction == fraction) {
		return;
	}

	this->curBandSizeFraction = fraction;
	this->onBandSizeChanged();
}
