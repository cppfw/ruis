#include "fraction_band_widget.hpp"

using namespace morda;

void fraction_band_widget::on_band_changed(){
	if (this->band_change_handler){
		this->band_change_handler(*this);
	}
}

void fraction_band_widget::set_band_fraction(real fraction){
	if (this->curBandSizeFraction == fraction){
		return;
	}

	this->curBandSizeFraction = fraction;
	this->on_band_changed();
}
