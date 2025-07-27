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

#include "../../util/content_wrapping.hpp"
#include "../../util/oriented.hpp"
#include "../container.hpp"

namespace ruis {

/*
The tile_area arranges its content widgets (tiles) either vertially or horizontally.
Common borders of the contained tiles are draggable with mouse pointer.
*/
class tiling_area :
	virtual public widget, //
	public ruis::oriented,
	public content_wrapping,
	private ruis::container
{
public:
	const ruis::real min_tile_size;
	const ruis::real dragger_size;

	struct parameters {
		styled<color> dragger_color;
	};

private:
	parameters params;

public:
	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
		parameters tiling_area_params;
	};

	tiling_area(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		ruis::widget_list children
	);

	void on_lay_out() override;

	ruis::vector2 measure(const ruis::vector2& quotum) const override;

	// override in order to avoid invalidation of layout when children list changes,
	// because default implementation of this method invalidates layout
	void on_children_change() override
	{
		// do nothing
	}

private:
};

namespace make {
utki::shared_ref<ruis::tiling_area> tiling_area(
	utki::shared_ref<ruis::context> context, //
	ruis::tiling_area::all_parameters params,
	ruis::widget_list children
);
} // namespace make

} // namespace ruis
