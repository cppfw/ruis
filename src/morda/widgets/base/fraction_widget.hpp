#pragma once

#include "../widget.hpp"

namespace morda{
class fraction_widget : public virtual widget{
	float fraction_v = 0; // current position from 0 to 1
protected:
	fraction_widget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
	{}

	virtual void on_fraction_change();
public:
	std::function<void(fraction_widget&)> fraction_change;

	real fraction()const noexcept{
		return this->fraction_v;
	}

	void set_fraction(float newFraction);
};
}