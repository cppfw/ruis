/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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