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

#include "padding.hpp"

#include <utki/views.hpp>

#include "../label/gap.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

padding::padding(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	widget_list children
) :
	padding(
		context, //
		params,
		// clang-format off
		m::container(
			context,
			{
				.params = [&]() {
					// pile layout by default
					if (auto& l = params.params.container.layout; !l) {
						l = layout::pile;
					}
					return std::move(params.params.container);
				}()},
			std::move(children)
		)
		// clang-format on
	)
{}

padding::padding(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::container> content_container
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// clang-format off
	container(
		context,
		{},
		{
			content_container
		}
	),
	// clang-format on
	containing_widget(
		context, //
		content_container.get()
	),
	params(std::move(params.params.specific))
{}

sides<real> padding::get_min_borders() const noexcept
{
	return {0, 0, 0, 0};
}

vec2 padding::measure(
	const vec2& quotum, //
	const r4::vector2<measure_mode>& mode
) const
{
	if (mode[0] == measure_mode::exactly && mode[1] == measure_mode::exactly) {
		return quotum;
	}

	auto borders = this->get_actual_borders();

	auto borders_left_top = borders.left_top();
	auto borders_right_bottom = borders.right_bottom();

	vec2 borderless_quotum;
	r4::vector2<measure_mode> content_mode;
	// TODO: use utki::zip
	for (unsigned i = 0; i != borderless_quotum.size(); ++i) {
		content_mode[i] = mode[i];
		if (mode[i] == measure_mode::exactly) {
			using std::max;
			borderless_quotum[i] = max(real(0), quotum[i] - borders_left_top[i] - borders_right_bottom[i]);
		} else {
			utki::assert(mode[i] == measure_mode::at_most);
			borderless_quotum[i] = measure_infinite_quotum;
		}
	}

	vec2 ret;
	// TODO: use utki::zip
	for (unsigned i = 0; i != ret.size(); ++i) {
		ret[i] = (mode[i] == measure_mode::exactly) ? quotum[i] : real(0);
	}

	auto content_min_dims = this->get_container().measure(
		borderless_quotum, //
		content_mode
	);
	// TODO: use utki::zip
	for (unsigned i = 0; i != ret.size(); ++i) {
		if (mode[i] == measure_mode::at_most) {
			ret[i] = borders_left_top[i] + content_min_dims[i] + borders_right_bottom[i];
		}
	}

	return ret;
}

void padding::on_lay_out()
{
	auto borders = this->get_actual_borders();

	vec2 content_dims = max(real(0), this->rect().d - borders.dims());

	auto& c = this->get_container();

	c.move_to(borders.left_top());
	c.resize(content_dims);
}

utki::shared_ref<ruis::padding> ruis::make::padding(
	const utki::shared_ref<context>& context, //
	padding::all_parameters params,
	widget_list children
)
{
	if (auto& l = params.params.container.layout; !l) {
		l = ruis::layout::pile;
	}

	return utki::make_shared<ruis::padding>(
		context, //
		std::move(params),
		std::move(children)
	);
}

void padding::set_borders(sides<styled<length>> borders)
{
	if (this->params.borders == borders) {
		return;
	}

	this->params.borders = borders;
	this->on_borders_change();
}

sides<real> padding::get_actual_borders() const noexcept
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
		if (b.get().is_undefined()) {
			a = m;
		} else {
			a = b.get().get(this->context);
		}
	}

	return actual_borders;
}

void padding::on_borders_change()
{
	this->invalidate_layout();
}
