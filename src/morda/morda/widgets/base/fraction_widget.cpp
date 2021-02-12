#include "fraction_widget.hpp"

using namespace morda;

void fraction_widget::set_fraction(real fraction, bool notify_change){
	using std::min;
	using std::max;
	fraction = max(fraction, real(0)); // clamp bottom
	fraction = min(fraction, real(1)); // clamp top

	// in case of nan or inf
	if(!(0 <= fraction && fraction <= 1)){
		fraction = 0;
	}

	ASSERT_INFO(0 <= fraction && fraction <= 1, fraction)

	if(this->fraction_v == fraction){
		return;
	}

	this->fraction_v = fraction;

	if(notify_change){
		this->on_fraction_change();
	}else{
		auto f = std::move(this->fraction_change_handler);
		this->on_fraction_change();
		this->fraction_change_handler = std::move(f);
	}
}

void fraction_widget::on_fraction_change(){
	if(this->fraction_change_handler){
		this->fraction_change_handler(*this);
	}
}
