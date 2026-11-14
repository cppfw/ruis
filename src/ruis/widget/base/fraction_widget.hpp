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

#include "../widget.hpp"

namespace ruis {

/**
 * @brief A base class for widgets which display a value in the [0:1] range.
 */
class fraction_widget : public virtual widget
{
private:
	real fraction = 0; // current position from 0 to 1

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param fraction - initial fraction value in [0:1] range.
	 */
	fraction_widget(
		const utki::shared_ref<ruis::context>& context, //
		real fraction
	) :
		widget(context, {}, {}),
		fraction(fraction)
	{}

	/**
	 * @brief Invoked when the fraction value has changed.
	 */
	virtual void on_fraction_change();

public:
	/**
	 * @brief Fraction change handler.
	 * Invoked when the fraction value has changed.
	 */
	std::function<void(fraction_widget&)> fraction_change_handler;

	/**
	 * @brief Get fraction value.
	 * @return current fraction value in [0:1] range.
	 */
	real get_fraction() const noexcept
	{
		return this->fraction;
	}

	/**
	 * @brief Set fraction value.
	 * @param fraction - the fraction value to set, must be from [0:1].
	 * @param notify_change - whether to call the fraction_change_handler or not.
	 */
	void set_fraction(
		real fraction, //
		bool notify_change = true
	);
};
} // namespace ruis