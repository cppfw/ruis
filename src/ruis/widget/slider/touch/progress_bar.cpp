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

#include "progress_bar.hpp"

#include <algorithm>

#include "../../../context.hpp"

using namespace ruis::touch;

progress_bar::progress_bar( //
	const utki::shared_ref<ruis::context>& context,
	all_parameters params
) :
	widget( //
		context,
		[&]() {
			// if the vertical (long) dimension is not set to a concrete value (undefined or 'min')
			// then default it to the style's 'len_gap_small'
			if (ruis::dim::is_min_type(params.layout_params.dims.y().get_type())) {
				params.layout_params.dims.y() = context.get().style().get_len_gap_small();
			}
			return std::move(params.layout_params);
		}(),
		std::move(params.widget)
	),
	fraction_widget( //
		context,
		params.params.fraction
	),
	params([&]() {
		auto& p = params.params.specific;
		// resolve the colors from the style when undefined
		if (p.background_color.get().is_undefined()) {
			p.background_color = context.get().style().get_color_secondary();
		}
		if (p.bar_color.get().is_undefined()) {
			p.bar_color = context.get().style().get_color_special();
		}
		return std::move(params.params);
	}()),
	vao(context.get().renderer)
{}

void progress_bar::on_fraction_change()
{
	this->fraction_widget::on_fraction_change();

	// the bar width depends on the fraction, so re-render the widget
	this->clear_cache();
}

void progress_bar::on_resize()
{
	this->widget::on_resize();

	// the capsule thickness (diameter) is the height of the widget
	this->vao.set(this->rect().d.y());
}

void progress_bar::render(const ruis::mat4& matrix) const
{
	const auto w = this->rect().d.x();
	const auto h = this->rect().d.y();

	using std::max;

	// the background (track) capsule, it fills the whole widget
	this->vao.render(
		matrix, //
		this->params.specific.background_color.get(),
		w
	);

	// the progress (bar) capsule, it grows from the left following the fraction:
	// at 0% it's just a circle (width == height) and at 100% it's the whole bar
	const auto bar_w = max(h, h + (w - h) * this->get_fraction());
	this->vao.render(
		matrix, //
		this->params.specific.bar_color.get(),
		bar_w
	);
}

utki::shared_ref<ruis::touch::progress_bar> ruis::touch::make::progress_bar(
	const utki::shared_ref<ruis::context>& context, //
	ruis::touch::progress_bar::all_parameters params
)
{
	return utki::make_shared<ruis::touch::progress_bar>(
		context, //
		std::move(params)
	);
}
