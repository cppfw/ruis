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

#include "animation.hpp"
#include "easing.hpp"

namespace ruis {

class eased_animation : public animation
{
	std::function<real(real factor)> easing;

protected:
	void on_update(real factor) override;

public:
	eased_animation( //
		utki::shared_ref<ruis::updater> updater,
		uint32_t duration_ms,
		decltype(easing) easing = easing::linear
	);
};

} // namespace ruis
