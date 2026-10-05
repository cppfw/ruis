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

#include "capsule.hpp"

#include <algorithm>

#include "../../context.hpp"

using namespace ruis;

capsule::capsule( //
	const utki::shared_ref<ruis::context>& context,
	all_parameters params,
	widget_list children
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// clang-format off
	padding(
		context,
		{
			.params = std::move(params.params.padding)
		},
		std::move(children)
	),
	// clang-format on
	params(std::move(params.params.specific)),
	stroke_vao(context.get().renderer),
	fill_vao(context.get().renderer)
{
	this->update_vaos();
}

void capsule::set_fill_color(styled<ruis::color> color)
{
	if (this->params.fill_color == color) {
		return;
	}

	this->params.fill_color = std::move(color);
	this->clear_cache();
}

void capsule::set_stroke_color(styled<ruis::color> color)
{
	if (this->params.stroke_color == color) {
		return;
	}

	this->params.stroke_color = std::move(color);
	this->clear_cache();
}

void capsule::set_stroke_width(styled<length> width)
{
	if (this->params.stroke_width == width) {
		return;
	}

	this->params.stroke_width = std::move(width);
	this->update_vaos();
	this->clear_cache();
}

void capsule::render(const ruis::mat4& matrix) const
{
	const auto w = this->rect().d[0];

	if (this->has_stroke()) {
		const auto b = this->params.stroke_width.get().get(this->context);

		// the stroke
		this->stroke_vao.render(
			matrix, //
			this->get_stroke_color(),
			w
		);

		// the fill, inset by the border
		ruis::mat4 matr(matrix);
		matr.translate(b, b);

		this->fill_vao.render(
			matr, //
			this->get_fill_color(),
			w - 2 * b
		);
	} else {
		this->fill_vao.render(
			matrix, //
			this->get_fill_color(),
			w
		);
	}

	this->padding::render(matrix);
}

void capsule::on_resize()
{
	this->update_vaos();
	this->padding::on_resize();
}

void capsule::update_vaos()
{
	using std::max;

	const auto h = max(this->rect().d[1], real(0));

	if (this->has_stroke()) {
		const auto b = this->params.stroke_width.get().get(this->context);

		this->stroke_vao.set(h);
		this->fill_vao.set(max(h - 2 * b, real(0)));
	} else {
		this->fill_vao.set(h);
	}
}

utki::shared_ref<ruis::capsule> ruis::make::capsule(
	const utki::shared_ref<ruis::context>& context, //
	ruis::capsule::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::capsule>(
		context, //
		std::move(params),
		std::move(children)
	);
}
