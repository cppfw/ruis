#include "FractionBandWidget.hpp"

using namespace morda;

void FractionBandWidget::onBandSizeChanged(){
	if (this->band_change_handler){
		this->band_change_handler(*this);
	}
}

void FractionBandWidget::setBandSizeFraction(real fraction){
	if (this->curBandSizeFraction == fraction){
		return;
	}

	this->curBandSizeFraction = fraction;
	this->onBandSizeChanged();
}
