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

#include "../../label/padding.hpp"

using namespace ruis::touch;

tab_group::tab_group(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	widget_list children
) :
	widget(
		context, //
		std::move(params.layout),
		std::move(params.widget)
	),
	choice_group(
		context, //
		{
			.layout = std::move(params.layout),
			.widget = std::move(params.widget),
			.params = [&](){
				if (auto& c = params.params.container.layout; !c) {
					c = ruis::layout::row;
				}
				return std::move(params.params.container);
			}()
		},
		[&]() {
			for (auto& c : children) {
				// clang-format off
				c = ruis::make::padding(
					context,
					{
						// use same layout params for the padding, except the dims
						.layout = [&]() {
							auto lp = c.get().get_layout_params_const();
							lp.dims = {ruis::dim::max, ruis::dim::max};
							return lp;
						}(),
						.params{
							.specific{
								.borders = {context.get().style().get_len_gap()
								} // TODO: get from params, should be same as selector_gap
							}
						}
					},
					{c}
				);
				// clang-format on
			}

			return std::move(children);
		}()
	),
	selector_vao(
		context.get().renderer, //
		{.corner_radii =
			{
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "TODO: get from params")
				ruis::length::make_pp(10).get(context) // TODO: get rounded corners from params
			}}
	),
	params([&]() {
		if(auto& c = params.params.specific.background_color; c.get().is_undefined()){
			c = context.get().style().get_color_panel();
		}
		if (auto& c = params.params.specific.selector_color; c.get().is_undefined()) {
			c = context.get().style().get_color_secondary();
		}
		return std::move(params.params.specific);
	}())
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
			this->params.background_color.get()
		);
	}

	// render selector
	if (auto active_tab = this->get_active().lock()) {
		auto pos = active_tab.get()->get_pos_in_ancestor({0}, this);

		ruis::mat4 matr(matrix);
		matr.translate(pos);
		this->selector_vao.render(
			matr, //
			active_tab->rect().d,
			this->params.selector_color.get()
		);
	}

	this->choice_group::render(matrix);
}

utki::shared_ref<ruis::touch::tab_group> ruis::touch::make::tab_group(
	const utki::shared_ref<ruis::context>& context, //
	ruis::touch::tab_group::all_parameters params,
	widget_list tabs
)
{
	return utki::make_shared<ruis::touch::tab_group>(
		context, //
		std::move(params),
		std::move(tabs)
	);
}
