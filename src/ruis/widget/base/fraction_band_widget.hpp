/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

/**
 * @brief A fraction widget which displays a band of a given width at a given
 * position within the [0:1] range.
 */
class fraction_band_widget : public fraction_widget
{
private:
	real band_fraction = 0; // current bar size factor from 0 to 1

protected:
	/**
	 * @brief Fraction band widget parameters.
	 */
	struct parameters {
		/**
		 * @brief Band position in the [0:1] range.
		 */
		real fraction = 0;

		/**
		 * @brief Band size as a fraction of 0 to 1.
		 */
		real band_fraction = 0;
	};

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - fraction band widget parameters.
	 */
	fraction_band_widget(
		const utki::shared_ref<ruis::context>& context, //
		parameters params
	);

	/**
	 * @brief Invoked when the band position, band size, or widget size has changed.
	 * The default implementation calls the band_change_handler.
	 */
	virtual void on_band_change();

public:
	/**
	 * @brief Band change handler.
	 * Invoked when the band position, band size, or widget size has changed.
	 */
	std::function<void(fraction_band_widget& s)> band_change_handler;

	/**
	 * @brief Set band size.
	 * @param fraction - band size as a fraction of 0 to 1.
	 */
	void set_band_fraction(real fraction);

	/**
	 * @brief Get band size.
	 * @return band size as a fraction of 0 to 1.
	 */
	real get_band_fraction() const noexcept
	{
		return this->band_fraction;
	}
};

} // namespace ruis
