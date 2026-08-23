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

#include "../base/color_widget.hpp"
#include "../../paint/ellipse_vao.hpp"

#include "padding.hpp"

namespace ruis {

class ellipse :
	public padding, //
	public color_widget
{
	paint::ellipse_vao vao;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		color_widget::parameters color_params;
	};

	ellipse(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	ellipse(const ellipse&) = delete;
	ellipse& operator=(const ellipse&) = delete;

	ellipse(ellipse&&) = delete;
	ellipse& operator=(ellipse&&) = delete;

	~ellipse() override = default;

	void render(const ruis::mat4& matrix) const override;

	void on_resize()override;

public:
	void update_vao();
};

namespace make{
utki::shared_ref<ruis::ellipse> ellipse(
	utki::shared_ref<ruis::context> context, //
	ruis::ellipse::all_parameters params,
	widget_list children = {}
);
}

} // namespace ruis
