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

#include "../base/frame_widget.hpp"

namespace ruis {

class margins : public frame_widget
{
	widget& left;
	widget& top;
	widget& right;
	widget& bottom;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		frame_widget::parameters frame_params;
	};

	margins(
		utki::shared_ref<ruis::context> context,
		all_parameters params,
		utki::span<const utki::shared_ref<ruis::widget>> children
	);

	void on_borders_change() override;
};

namespace make {
inline utki::shared_ref<margins> margins(
	utki::shared_ref<context> context,
	margins::all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::margins>(std::move(context), std::move(params), children);
}
} // namespace make

} // namespace ruis
