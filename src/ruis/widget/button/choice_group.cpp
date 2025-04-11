/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "choice_group.hpp"

#include "../../layout/pile_layout.hpp"

using namespace ruis;

choice_group::choice_group(
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
			.container_params = std::move(params.container_params)
		},
		std::move(children)
	)
// clang-format on
{}

bool choice_group::is_active(const widget& w) const noexcept
{
	widget* aw = this->active_choice_button.lock().get();
	return aw == &w;
}

void choice_group::set_active_choice_button(std::weak_ptr<choice_button> cb)
{
	auto oldactive = this->active_choice_button.lock();

	if (cb.lock() == oldactive) {
		return;
	}

	this->active_choice_button = cb;

	if (oldactive) {
		oldactive->set_pressed(false);
	}
}

utki::shared_ref<ruis::choice_group> ruis::make::choice_group(
	utki::shared_ref<ruis::context> context, //
	ruis::choice_group::all_parameters params,
	widget_list children
)
{
	if (!params.container_params.layout) {
		params.container_params.layout = ruis::layout::column;
	}

	return utki::make_shared<ruis::choice_group>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}