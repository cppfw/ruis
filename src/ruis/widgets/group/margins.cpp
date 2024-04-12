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

#include "margins.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

margins::margins(
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children
) :
	widget(
		std::move(context),
		{
			.widget_params = std::move(params.widget_params)
}
	),
	frame_widget(
		this->context,
		{//
		 .container_params = {.layout = layout::row},
		 .frame_params = std::move(params.frame_params)
		},
		{//
		 m::widget(this->context, {.widget_params = {.id = "ruis_left"s}}),
		 m::container(
			 this->context,
			 {//
			  .widget_params =
				  {//
				   .lp =
					   {//
						.dims = {lp::max, lp::max},
						.weight = 1
					   }
				  },
			  .container_params = {.layout = layout::column}
			 },
			 {//
			  m::widget(this->context, {.widget_params = {.id = "ruis_top"s}}),
			  m::container(
				  this->context,
				  {//
				   .widget_params =
					   {//
						.id = "ruis_content"s,
						.lp =
							{//
							 .dims = {lp::max, lp::max},
							 .weight = 1
							}
					   },
				   .container_params = {.layout = layout::pile}
				  },
				  {}
			  ),
			  m::widget(this->context, {.widget_params = {.id = "ruis_bottom"s}})
			 }
		 ),
		 m::widget(this->context, {.widget_params = {.id = "ruis_right"s}})
		},
		children
	),
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

	std::cout << "borders = " << b << std::endl;

	this->left.get_layout_params().dims.x() = b.left();
	this->top.get_layout_params().dims.y() = b.top();
	this->right.get_layout_params().dims.x() = b.right();
	this->bottom.get_layout_params().dims.y() = b.bottom();
}
