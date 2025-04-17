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
			m::gap(this->context,
				{
					.widget_params{
						.id = "ruis_left"s
					}
				}
			),
			m::container(this->context,
				{
					.layout_params{
						.dims = {dim::max, dim::max},
						.weight = 1
					},
					.container_params{
						.layout = layout::column
					}
				},
				{
					m::gap(this->context,
						{
							.widget_params{
								.id = "ruis_top"s
							}
						}
					),
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
					),
					m::gap(this->context,
						{
							.widget_params{
								.id = "ruis_bottom"s
							}
						}
					)
				}
			),
			m::gap(this->context,
				{
					.widget_params{
						.id = "ruis_right"s
					}
				}
			)
		}
	),
	// clang-format on
	left(this->get_widget("ruis_left"sv)), //
	top(this->get_widget("ruis_top"sv)), //
	right(this->get_widget("ruis_right"sv)), //
	bottom(this->get_widget("ruis_bottom"sv))
{
	this->margins::on_borders_change();
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks, "false positive")

void margins::on_borders_change()
{
	const auto& b = this->get_borders();

	// std::cout << "borders = " << b << std::endl;

	this->left.get_layout_params().dims.x() = b.left();
	this->top.get_layout_params().dims.y() = b.top();
	this->right.get_layout_params().dims.x() = b.right();
	this->bottom.get_layout_params().dims.y() = b.bottom();
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
