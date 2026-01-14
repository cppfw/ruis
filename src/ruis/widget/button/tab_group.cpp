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

#include "tab_group.hpp"

#include "../../util/util.hpp"

#include "tab.hpp"

using namespace ruis;

tab_group::tab_group(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list tabs
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	choice_group(
		this->context, //
		{
			.container_params = std::move(params.container_params)
		},
		std::move(tabs)
	)
// clang-format on
{
	// TODO: allow setting filler from params
	if (!this->filler) {
		this->set_filler(this->context.get().loader().load<res::image>("ruis_img_tabs_filler").to_shared_ptr());
	}
}

void tab_group::set_filler(std::shared_ptr<const res::image> filler)
{
	this->filler = std::move(filler);
	this->filler_texture = this->filler->get(this->context.get().units).to_shared_ptr();
}

ruis::vector2 tab_group::measure(const ruis::vector2& quotum) const
{
	vector2 ret(quotum);
	using std::max;
	ret = max(ret, real(0)); // clamp bottom

	real length = 0;

	sides<real> prev_borders = real(0);

	for (const auto& c : this->children()) {
		const auto& lp = c.get().get_layout_params_const();

		auto t = dynamic_cast<const ruis::tab*>(&c.get());
		if (!t) {
			throw std::logic_error(
				"Non-tab widget added to tab_group, only tab widgets are allowed to be added to tab_group"
			);
		}

		ruis::vector2 d;

		for (unsigned j = 0; j != d.size(); ++j) {
			const auto& dim = lp.dims[j].get();
			switch (dim.get_type()) {
				case ruis::dim::type::fill:
					[[fallthrough]];
				case ruis::dim::type::max:
					throw std::logic_error("'max' or 'fill' encountered in layout parameters for tab_group container");
				case ruis::dim::type::undefined:
					[[fallthrough]];
				case ruis::dim::type::min:
					d[j] = -1;
					break;
				case ruis::dim::type::length:
					d[j] = dim.get_length().get(this->context);
					break;
			}
		}

		d = c.get().measure(d);

		length += d.x();

		using std::min;
		using std::max;

		auto borders = t->get_actual_borders();
		length -= min(prev_borders.right(), borders.left());
		prev_borders = borders;

		if (quotum.y() < 0) {
			ret.y() = max(ret.y(), d.y()); // clamp bottom
		}
	}

	if (quotum.x() < 0) {
		ret.x() = length;
	}

	return ret;
}

void tab_group::on_lay_out()
{
	real pos = 0;

	sides<real> prev_borders = 0;

	for (const auto& c : this->children()) {
		auto dim = dims_for_widget(c.get(), this->rect().d);
		c.get().resize(dim);

		auto t = dynamic_cast<ruis::tab*>(&c.get());
		if (!t) {
			throw std::logic_error(
				"Non-tab widget added to tab_group, only tab widgets are allowed to be added to tab_group"
			);
		}

		auto borders = t->get_actual_borders();

		using std::min;
		using std::round;

		pos -= min(prev_borders.right(), borders.left());
		c.get().move_to(vector2(
			pos, //
			// align bottom
			round(this->rect().d.y() - c.get().rect().d.y())
		));
		pos += dim.x();

		prev_borders = borders;
	}
}

void tab_group::render(const ruis::matrix4& matrix) const
{
	auto active_tab = this->get_active().lock();

	for (const auto& w : this->children()) {
		if (w.to_shared_ptr() != active_tab) {
			this->render_child(matrix, w.get());
		}
	}

	if (active_tab) {
		this->render_child(matrix, *active_tab);
	}

	// render filler
	if (this->children().size() != 0) {
		real ce = this->children().back().get().rect().x2();
		real l = this->rect().d.x() - ce;
		if (l > 0) {
			matrix4 m(matrix);
			m.translate(ce, this->rect().d.y() - real(this->filler_texture->dims().y()));
			m.scale(l, real(this->filler_texture->dims().y()));
			this->context.get().ren().render(
				m, //
				*this->filler_texture
			);
		}
	}
}
