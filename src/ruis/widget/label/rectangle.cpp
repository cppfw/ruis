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

#include "rectangle.hpp"

#include "../../context.hpp"

using namespace ruis;

rectangle::rectangle( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	padding( //
		this->context,
		{
			.container_params = std::move(params.container_params),
			.padding_params = std::move(params.padding_params)
		},
		std::move(children)
	),
	// clang-format on
	color_widget( //
		this->context,
		std::move(params.color_params)
	),
	params(std::move(params.rectangle_params)),
	fill_vao(this->context.get().renderer)
{
	this->update_vaos();
}

void rectangle::render(const ruis::matrix4& matrix) const
{
	this->fill_vao.render(
		matrix, //
		this->rect().d,
		this->get_current_color()
	);

	this->padding::render(matrix);
}

void rectangle::update_vaos()
{
	sides<real> radii;
	std::transform(
		this->params.corner_radii.begin(),
		this->params.corner_radii.end(),
		radii.begin(),
		[this](const auto& r) -> real {
			if (r.get().is_undefined()) {
				return 0;
			}
			return r.get().get(this->context);
		} //
	);

	this->fill_vao.set(radii, 0);
}
