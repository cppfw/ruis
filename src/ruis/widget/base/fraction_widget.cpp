/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "fraction_widget.hpp"

using namespace ruis;

void fraction_widget::set_fraction(
	real fraction, //
	bool notify_change
)
{
	using std::min;
	using std::max;
	fraction = max(fraction, real(0)); // clamp bottom
	fraction = min(fraction, real(1)); // clamp top

	// in case of nan or inf
	if (!(0 <= fraction && fraction <= 1)) {
		fraction = 0;
	}

	ASSERT(0 <= fraction && fraction <= 1, [&](auto& o) {
		o << fraction;
	})

	if (this->params.fraction == fraction) {
		return;
	}

	this->params.fraction = fraction;

	if (notify_change) {
		this->on_fraction_change();
	} else {
		// We still need to call the on_fraction_change() callback, so to make sure the handler
		// is not invoked by default implementation of the callback, unset it for the time of calling the callback.
		auto f = std::move(this->fraction_change_handler);

		// Moving-from does not guarantee it will be nulled out, so set to nullptr explicitly.
		// For example it is needed on Macos (clang + libc++).
		this->fraction_change_handler = nullptr;

		this->on_fraction_change();
		this->fraction_change_handler = std::move(f);
	}
}

void fraction_widget::on_fraction_change()
{
	if (this->fraction_change_handler) {
		// TODO: on macos clang-tidy gives false-positive finding here
		// NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
		this->fraction_change_handler(*this);
	}
}
