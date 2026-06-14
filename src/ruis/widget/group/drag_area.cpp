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

#include "drag_area.hpp"

using namespace ruis;

drag_area::drag_area(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
    container(this->context,
        {
            .container_params = {
                .layout = ruis::layout::trivial
            }
        },
        std::move(children)
    ),
	// clang-format on
	cur_dots_per_pp(this->context.get().units.dots_per_pp())
{}

void drag_area::on_reload()
{
	if (this->cur_dots_per_pp != this->context.get().units.dots_per_pp()) {
		real old_dots_per_pp = this->cur_dots_per_pp;
		this->cur_dots_per_pp = this->context.get().units.dots_per_pp();

		real scale_factor = this->cur_dots_per_pp / old_dots_per_pp;

		for (auto& w_ref : this->children()) {
			auto& w = w_ref.get();
			w.move_to(w.rect().p * scale_factor);
			w.resize(w.rect().d * scale_factor);
		}
	}

	this->container::on_reload();
}

utki::shared_ref<drag_area> ruis::make::drag_area(
	utki::shared_ref<ruis::context> context, //
	ruis::drag_area::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::drag_area>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
