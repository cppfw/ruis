#include "FractionWidget.hpp"


using namespace morda;

void FractionWidget::setFraction(float newFraction) {
	real fraction = utki::clampedRange(newFraction, 0.0f, 1.0f);

	//in case of nan or inf
	if (!(0 <= fraction && fraction <= 1)) {
		fraction = 0;
	}

	ASSERT_INFO(0 <= fraction && fraction <= 1, fraction)

	if (this->fraction_v == fraction) {
		return;
	}

	this->fraction_v = fraction;

	this->onFractionChange();
}

void FractionWidget::onFractionChange() {
	if (this->fractionChange) {
		this->fractionChange(*this);
	}
}
