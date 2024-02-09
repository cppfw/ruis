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

#include "make.hpp"

using namespace ruis::make;

utki::shared_ref<ruis::widget> ruis::make::widget(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters params
)
{
	return utki::make_shared<ruis::widget>(std::move(context), std::move(params));
}

utki::shared_ref<ruis::widget> ruis::make::container(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::container::parameters params,
	const std::vector<utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::container>(
		std::move(context),
		std::move(widget_params),
		std::move(params),
		children
	);
}
