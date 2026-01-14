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

#pragma once

#include "../choice_group.hpp"

namespace ruis::touch {

class tab_group : public choice_group
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
	};

	tab_group(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list tabs
	);

	void render(const ruis::matrix4& matrix) const override;
};

namespace make {
/**
 * @brief Create touch tab group.
 * Default container layout is row.
 * @param context - ruis context.
 * @param params - touch tab group parameters.
 * @param tabs - tabs to add to the tab group.
 * @return Shared reference to the created tab group.
 */
utki::shared_ref<ruis::touch::tab_group> tab_group(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::tab_group::all_parameters params,
	widget_list tabs = {}
);
} // namespace make

} // namespace ruis::touch
