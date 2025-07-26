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

#include "tree_view.hpp"

namespace ruis {

class table_tree_view :
	virtual public ruis::widget, //
	private ruis::container
{
public:
	/**
	 * @brief A class for accessing the tree data model.
	 * This class provides access to the tree data model and constructs
	 * widgets representing the tree items.
	 */
	class provider : private tree_view::provider
	{
	public:
		provider(utki::shared_ref<ruis::context> context);

		/**
		 * @brief Create a table_tree_view item widget.
		 * The table_tree_view will call this mthod when it needs a widget for the given
		 * tree view item.
		 * The widget has to be a container and is supposed to have a row layout.
		 * Each child widget of the container widget will represent a cell in the table row.
		 * @param index - index of the item into the tree data to create widget for.
		 * @return A container widget with row layout containing table row cell widgets.
		 */
		virtual utki::shared_ref<ruis::container> get_container(utki::span<const size_t> index) = 0;
	};

	struct parameters {
		/**
		 * @brief Column header widgets.
		 * These widgets will be put inside of a ruis::container and arranged in a row.
		 */
		ruis::widget_list column_headers = {};
		std::shared_ptr<table_tree_view::provider> provider;
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
};

namespace make {
utki::shared_ref<ruis::table_tree_view> table_tree_view(
	utki::shared_ref<ruis::context> context, //
	ruis::table_tree_view::all_parameters params
);
} // namespace make

} // namespace ruis
