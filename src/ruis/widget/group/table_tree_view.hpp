/*
booh - Double record bookkeeping application

Copyright (C) 2025 Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <ruis/widget/container.hpp>
#include <ruis/widget/group/tree_view.hpp>

namespace ruis {

class table_tree_view :
	virtual public ruis::widget, //
	private ruis::container
{
public:
	struct parameters {
		/**
		 * @brief Column header widgets.
		 * These widgets will be put inside of a ruis::container with row layout.
		 */
		ruis::widget_list column_headers = {};
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
		parameters table_tree_view_params;
	};

	table_tree_view(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	class provider : protected ruis::tree_view::provider
	{
	public:
		provider(utki::shared_ref<ruis::context> context);

		/**
		 * @brief Create a tree_view item widget.
		 * The table_tree_view will call this mthod when it needs a widget for the given
		 * tree view item.
		 * The widget has to be a container and is supposed to have a row layout.
		 * Each child widget of the container widget will represent a cell in the table row.
		 * @param index - index of the item into the tree data to create widget for.
		 * @return A container widget with row layout containing table row cell widgets.
		 */
		virtual ruis::container get_widget(utki::span<const size_t> index) = 0;
	};
};

namespace make {
utki::shared_ref<ruis::table_tree_view> table_tree_view(
	utki::shared_ref<ruis::context> context, //
	ruis::table_tree_view::all_parameters params
);
} // namespace make

} // namespace ruis
