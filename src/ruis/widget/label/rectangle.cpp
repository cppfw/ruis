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

#include <algorithm>
#include <ranges>

#include "../../context.hpp"
#include "../../util/corner_radius.hpp"

using namespace ruis;

rectangle::rectangle( //
	const utki::shared_ref<ruis::context>& context,
	all_parameters params,
	widget_list children
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// clang-format off
	padding( //
		context,
		{
			.params = std::move(params.params.padding)
		},
		std::move(children)
	),
	// clang-format on
	params(std::move(params.params.specific)),
	fill_vao(context.get().renderer, {}),
	stroke_vao(context.get().renderer, {})
{
	this->update_vaos();
}

void rectangle::set_fill_color(styled<ruis::color> color)
{
	if (this->params.fill_color == color) {
		return;
	}

	this->params.fill_color = std::move(color);
	this->clear_cache();
}

void rectangle::set_stroke_color(styled<ruis::color> color)
{
	if (this->params.stroke_color == color) {
		return;
	}

	this->params.stroke_color = std::move(color);
	this->clear_cache();
}

void rectangle::set_stroke_width(styled<length> width)
{
	if (this->params.stroke_width == width) {
		return;
	}

	this->params.stroke_width = std::move(width);
	this->update_vaos();
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
			this->get_fill_color()
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
			this->get_fill_color()
		);
	}

	this->padding::render(matrix);
}

void rectangle::update_vaos()
{
	const auto& d = this->rect().d;
	const auto w = std::max(d[0], real(0));
	const auto h = std::max(d[1], real(0));

	// Resolve each corner to a concrete radius; undefined corners become 0.
	corners<real> radii;
	std::ranges::transform(
		this->params.corner_radii,
		radii.begin(),
		[this](const auto& r) -> real {
			const auto& len = r.get();
			return len.is_undefined() ? 0 : len.get(this->context);
		} //
	);

	if (this->has_stroke()) {
		const auto sw = this->params.stroke_width.get().get(this->context);

		// The fill is drawn inset by the stroke width, so fit its radii to the
		// smaller (w - sw) x (h - sw) box it is actually rendered into.
		corners<real> inner = radii;
		normalize_corner_radii(
			inner, //
			std::max(w - sw, real(0)), //
			std::max(h - sw, real(0)) //
		);
		this->fill_vao.set({.corner_radii = inner});

		// The stroke is drawn at the full widget size, so fit its radii to (w, h).
		corners<real> outer = radii;
		normalize_corner_radii(outer, w, h);
		this->stroke_vao.set({
			.corner_radii = outer, //
			.stroke_width = sw //
		});
	} else {
		// No stroke: the fill is drawn at the full widget size.
		normalize_corner_radii(radii, w, h);
		this->fill_vao.set({.corner_radii = radii});
	}
}

void rectangle::on_resize()
{
	// The normalized corner radii depend on the widget size (conflicting radii are
	// scaled down to fit the edges), so the VAOs must be rebuilt whenever the size
	// changes.
	this->update_vaos();
	this->padding::on_resize();
}

utki::shared_ref<ruis::rectangle> ruis::make::rectangle(
	const utki::shared_ref<ruis::context>& context,
	rectangle::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::rectangle>(
		context, //
		std::move(params),
		std::move(children)
	);
}
