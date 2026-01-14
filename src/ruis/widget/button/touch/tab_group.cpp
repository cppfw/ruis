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
	widget_list tabs
) :
	widget(
		std::move(context), //
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
		std::move(tabs)
	)
{}

void tab_group::render(const ruis::matrix4& matrix) const
{
	// TODO: render selector

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
