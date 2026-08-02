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

using namespace ruis::touch;

tab_group::tab_group(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	choice_group(
		this->context, //
		{
			.layout_params = std::move(params.layout_params),
			.widget_params = std::move(params.widget_params),
			.container_params = std::move(params.container_params),
		},
		std::move(children)
	),
	selector_vao(
		this->context.get().renderer, //
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "TODO: get from params")
		{ruis::length::make_pp(10).get(this->context)} // TODO: get from params
	),
	background_color(this->context.get().style().get_color_panel()), // TODO: get from params
	selector_color([&](){
		if(params.selector_color.get().is_undefined()){
			params.selector_color = this->context.get().style().get_color_secondary();
		}
		return std::move(params.selector_color);
	}()),
	selector_gap(this->context.get().style().get_len_gap()) // TODO: get from params
{}

void tab_group::render(const ruis::mat4& matrix) const
{
	// render background
	{
		auto& r = this->context.get().renderer.get();

		ruis::mat4 matr(matrix);
		matr.scale(this->rect().d);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao,
			this->background_color.get()
		);
	}

	// render selector
	if (auto active_tab = this->get_active().lock()) {
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "TODO: get from params")
		ruis::real gap = this->selector_gap.get().get(this->context);

		ruis::mat4 matr(matrix);
		matr.translate(active_tab->rect().p + vec2(gap));
		this->selector_vao.render(
			matr, //
			active_tab->rect().d - 2 * vec2(gap),
			this->selector_color.get()
		);
	}

	this->choice_group::render(matrix);
}

utki::shared_ref<ruis::touch::tab_group> ruis::touch::make::tab_group(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::tab_group::all_parameters params,
	widget_list tabs
)
{
	if (!params.container_params.layout) {
		params.container_params.layout = ruis::layout::row;
	}

	return utki::make_shared<ruis::touch::tab_group>(
		std::move(context), //
		std::move(params),
		std::move(tabs)
	);
}
