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

#include "table_list.hpp"
#include "tree_view.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::enable_shared_from_this is public via widget inheritance")
class table_tree_view :
	virtual public ruis::widget, //
	private container // TODO: inherit from table_list
{
public:
	/**
	 * @brief A class for accessing the tree data model.
	 * This class provides access to the tree data model and constructs
	 * widgets representing the tree items.
	 */
	class provider : private tree_view::provider_base
	{
		table_list::provider* list_provider = nullptr;

	public:
		provider(utki::shared_ref<ruis::context> context);

		/**
		 * @brief Create table row cell widgets for an item.
		 * The table_list will call this mthod when it needs widgets for the given list data item.
		 * A list of widgets is to be created.
		 * Each child widget will represent a cell in the table row.
		 * @param prefix_widgets - a list of tree_view specific widgets, i.e. colapse/expande button and path indicators.
		 *                         To be placed inside of a row.
		 * @return A list of widgets for table row cells.
		 */
		virtual widget_list get_row_widgets(
			utki::span<const size_t> index, //
			widget_list prefix_widgets
		) = 0;

		// TODO:
	};

	struct parameters {
		/**
		 * @brief Column header widgets.
		 * These widgets will be put inside of a horizontal ruis::tiling_area.
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
