/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../base/fraction_widget.hpp"

namespace ruis {

class fraction_band_widget : public fraction_widget
{
	float band_fraction = 0; // current bar size factor from 0 to 1

protected:
	fraction_band_widget(utki::shared_ref<ruis::context> c) :
		widget(std::move(c), widget::all_parameters{}),
		fraction_widget(this->context)
	{}

	fraction_band_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		fraction_widget(this->context, desc)
	{}

	virtual void on_band_change();

public:
	std::function<void(fraction_band_widget& s)> band_change_handler;

	void set_band_fraction(real fraction);

	real get_band_fraction() const noexcept
	{
		return this->band_fraction;
	}
};

} // namespace ruis
