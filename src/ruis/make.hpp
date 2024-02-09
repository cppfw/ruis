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

#include "container.hpp"
#include "lp.hpp"
#include "widget.hpp"

namespace ruis::make {

utki::shared_ref<ruis::widget> widget(utki::shared_ref<ruis::context> context, ruis::widget::parameters params);

utki::shared_ref<ruis::widget> container(
	utki::shared_ref<ruis::context> context,
	ruis::container::constructor_parameters params,
	const std::vector<utki::shared_ref<ruis::widget>> children
);

} // namespace ruis::make
