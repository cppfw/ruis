#pragma once

#include "../widget.hpp"

namespace morda{
class fraction_widget : public virtual widget{
	float fraction_v = 0; // current position from 0 to 1
protected:
	fraction_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc)
	{}

	virtual void on_fraction_change();
public:
	std::function<void(fraction_widget&)> fraction_change_handler;

	real fraction()const noexcept{
		return this->fraction_v;
	}

	/**
	 * @brief Set fraction value.
	 * @param fraction - the fraction value to set, must be from [0:1].
	 * @param notify_change - whether to call the fraction_change_handler or not.
	 */
	void set_fraction(real fraction, bool notify_change = true);
};
}