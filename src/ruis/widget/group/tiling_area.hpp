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

#include "../../util/content_wrapping.hpp"
#include "../../util/oriented.hpp"
#include "../container.hpp"

namespace ruis {

namespace internal {
class dragger;
} // namespace internal

/**
 * @brief Tiling area.
 * The tile_area arranges its content widgets (tiles) either vertially or horizontally.
 * Common borders of the adjacent contained tiles are draggable with mouse pointer.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::enable_shared_from_this is public via widget inheritance")
class tiling_area :
	virtual public widget, //
	public ruis::oriented,
	public content_wrapping,
	private ruis::container
{
	friend class ruis::internal::dragger;

public:
	/**
	 * @brief Minimal size of a tile in logitudinal direction of the tiling_area.
	 */
	const ruis::real min_tile_size;

	const ruis::real dragger_size;

	struct parameters {
		styled<color> dragger_color;
	};

private:
	parameters params;

public:
	struct all_parameters {
		ruis::layout::parameters layout_params;
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

	/**
	 * @brief Tiles resized handler.
	 * Invoked when tiles of the tile area have changed their sizes.
	 */
	std::function<void(tiling_area&)> tiles_resized_handler;

private:
	void notify_tiles_resized();
};

namespace make {
utki::shared_ref<ruis::tiling_area> tiling_area(
	utki::shared_ref<ruis::context> context, //
	ruis::tiling_area::all_parameters params,
	ruis::widget_list children
);
} // namespace make

} // namespace ruis
