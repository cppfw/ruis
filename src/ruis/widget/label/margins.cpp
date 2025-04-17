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

#include "margins.hpp"

#include <utki/views.hpp>

#include "../label/gap.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

margins::margins(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	frame_widget(
		this->context,
		{
			.layout = layout::row
		},
		std::move(params.frame_params),
		{
			m::container(this->context,
				{
					.layout_params{
						.dims = {dim::max, dim::max},
						.weight = 1
					},
					.widget_params{
						.id = "ruis_content"s
					},
					.container_params = std::move(params.container_params)
				},
				std::move(children)
			)
		}
	)
// clang-format on
{
	this->margins::on_borders_change();
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks, "false positive")

void margins::on_borders_change()
{
	// invalidate layout?
}

sides<real> margins::get_min_borders() const
{
	return {0, 0, 0, 0};
}

sides<real> margins::get_actual_borders() const
{
	auto min_borders = this->get_min_borders();
	const auto& borders = this->get_borders();

	sides<real> actual_borders;
	// clang-format off
	for (auto [m, b, a] :
		utki::views::zip(
			min_borders,
			borders,
			actual_borders
		)
	)
	// clang-format on
	{
		if (b.is_undefined()) {
			a = m;
		} else {
			a = b.get(this->context);
		}
	}

	return actual_borders;
}

vec2 margins::measure(const vec2& quotum) const
{
	auto actual_borders = this->get_actual_borders();

	auto borders_left_top = utki::make_span(actual_borders).subspan(0, 2);
	auto borders_right_bottom = utki::make_span(actual_borders).subspan(2, 2);

	vec2 ret = quotum;
	{
		auto content_min_dims = this->content().measure({-1, -1});
		// clang-format off
		for(auto [r, m, blt, brb] :
			utki::views::zip(
				ret,
				content_min_dims,
				borders_left_top,
				borders_right_bottom
			)
		)
		// clang-format on
		{
			if (r < 0) {
				r = blt + m + brb;
			}
		}
	}

	return ret;
}

void margins::on_lay_out()
{
	auto actual_borders = this->get_actual_borders();

	vec2 border_dims(
		actual_borders.left() + actual_borders.right(), //
		actual_borders.top() + actual_borders.bottom()
	);

	vec2 content_dims = max(real(0), this->rect().d - border_dims);

	auto& c = this->content();

	c.move_to(vec2(actual_borders.left(), actual_borders.top()));
	c.resize(content_dims);
}

utki::shared_ref<ruis::margins> ruis::make::margins(
	utki::shared_ref<context> context, //
	margins::all_parameters params,
	widget_list children
)
{
	if (!params.container_params.layout) {
		params.container_params.layout = ruis::layout::pile;
	}

	return utki::make_shared<ruis::margins>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
