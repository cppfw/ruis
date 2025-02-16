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

#pragma once

#include "../container.hpp"
#include "../widget.hpp"

namespace ruis {

// TODO: derive from frame_widget?
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class collapse_area :
	virtual public widget, //
	private container
{
	container& content_area;
	container& title_v;

public:
	collapse_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		string title;
	};

	collapse_area(
		utki::shared_ref<ruis::context> context,
		all_parameters params,
		utki::span<const utki::shared_ref<widget>> contents
	);

	container& content() noexcept
	{
		return this->content_area;
	}

	container& title() noexcept
	{
		return this->title_v;
	}
};

namespace make {
/**
 * @brief Create collapse_area widget.
 * Default layout is column.
 * @param context - ruis context.
 * @param params - collapse_area widget parameters.
 * @param contents - collapse area contents.
 * @return newly created 'collapse_area' widget.
 */
utki::shared_ref<ruis::collapse_area> collapse_area(
	utki::shared_ref<ruis::context> context, //
	ruis::collapse_area::all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> contents
);
} // namespace make

} // namespace ruis
