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

#pragma once

#include "spinner.hpp"

namespace ruis {

/**
 * @brief Busy spinner.
 * @param active - whether the busy indicator is initially active or not, can be true/false.
 */
class busy : public spinner
{
public:
	busy(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	void set_active(bool active);
};

} // namespace ruis
