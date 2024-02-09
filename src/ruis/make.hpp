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

inline utki::shared_ref<ruis::widget> widget(utki::shared_ref<ruis::context> context, ruis::widget::parameters params)
{
	return utki::make_shared<ruis::widget>(std::move(context), std::move(params));
}

inline utki::shared_ref<ruis::widget> container(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::container::parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::container>(
		std::move(context),
		std::move(widget_params),
		std::move(params),
		children
	);
}

} // namespace ruis::make
