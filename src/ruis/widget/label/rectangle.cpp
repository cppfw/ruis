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
	fill_vao(this->context.get().renderer, {}),
	stroke_vao(this->context.get().renderer, {})
{
	this->update_vaos();
}

void rectangle::set_stroke_color(styled<ruis::color> color)
{
	if (this->params.stroke_color == color) {
		return;
	}

	this->params.stroke_color = std::move(color);
	this->clear_cache();
}

void rectangle::render(const ruis::mat4& matrix) const
{
	const auto& dims = this->rect().d;

	if (this->has_stroke()) {
		const auto sw = this->params.stroke_width.get().get(this->context);
		const auto half_sw = sw / 2;

		ruis::mat4 matr(matrix);
		matr.translate(
			half_sw, //
			half_sw
		);

		this->fill_vao.render(
			matr, //
			dims - sw,
			this->get_current_color()
		);

		this->stroke_vao.render(
			matrix, //
			dims,
			this->params.stroke_color.get()
		);
	} else {
		this->fill_vao.render(
			matrix, //
			dims,
			this->get_current_color()
		);
	}

	this->padding::render(matrix);
}

void rectangle::update_vaos()
{
	corners<real> radii;
	std::ranges::transform(
		this->params.corner_radii,
		radii.begin(),
		[this](const auto& r) -> real {
			if (r.get().is_undefined()) {
				return 0;
			}
			return r.get().get(this->context);
		} //
	);

	this->fill_vao.set({.corner_radii = radii});

	if (this->has_stroke()) {
		this->stroke_vao.set({//
							  .corner_radii = radii, //
							  .stroke_width = this->params.stroke_width.get().get(this->context)
		});
	} else {
		this->stroke_vao.set({});
	}
}

utki::shared_ref<ruis::rectangle> ruis::make::rectangle(
	utki::shared_ref<ruis::context> context,
	rectangle::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::rectangle>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
