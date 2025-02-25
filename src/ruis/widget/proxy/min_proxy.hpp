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

#include "../container.hpp"

namespace ruis {

class min_proxy : virtual public ruis::widget
{
	mutable std::weak_ptr<const ruis::widget> target;

public:
	struct parameters {
		std::string root_id;
		std::vector<std::string> target_id_path;
	};

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		min_proxy::parameters min_proxy_params;
	};

private:
	mutable parameters params;

public:
	min_proxy(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	vector2 measure(const vector2& quotum) const override;
};

namespace make {
inline utki::shared_ref<ruis::min_proxy> min_proxy( //
	utki::shared_ref<ruis::context> context,
	min_proxy::all_parameters params
)
{
	return utki::make_shared<ruis::min_proxy>(
		std::move(context), //
		std::move(params)
	);
}

} // namespace make

} // namespace ruis
