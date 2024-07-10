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
protected:
	struct parameters {
		float band_fraction = 0; // current bar size factor from 0 to 1
	};

private:
	parameters params;

protected:
	// TODO: remove?
	struct all_parameters {
		fraction_widget::parameters fraction_params;
		parameters fraction_band_params;
	};

protected:
	fraction_band_widget(utki::shared_ref<ruis::context> c, all_parameters params);

	virtual void on_band_change();

public:
	std::function<void(fraction_band_widget& s)> band_change_handler;

	void set_band_fraction(real fraction);

	real get_band_fraction() const noexcept
	{
		return this->params.band_fraction;
	}
};

} // namespace ruis
