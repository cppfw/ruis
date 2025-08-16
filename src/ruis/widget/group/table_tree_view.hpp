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
	private table_list
{
	class table_list_provider_for_table_tree_view;

public:
	/**
	 * @brief A class for accessing the tree data model.
	 * This class provides access to the tree data model and constructs
	 * widgets representing the tree items.
	 */
	class provider : public tree_view::provider_base
	{
		friend class table_list_provider_for_table_tree_view;

		table_list::provider* list_provider = nullptr;

		ruis::widget_list list_get_row_widgets(size_t index);

	public:
		provider(utki::shared_ref<ruis::context> context);

		/**
		 * @brief Create table row cell widgets for an item.
		 * The table_list will call this mthod when it needs widgets for the given list data item.
		 * A list of widgets is to be created.
		 * Each child widget will represent a cell in the table row.
		 * @param index - index path of the item to get row widgets for.
		 * @return A list of widgets for table row cells.
		 */
		virtual widget_list get_row_widgets(utki::span<const size_t> index) = 0;

		// TODO:

	private:
		void on_list_model_changed() override;
	};

	struct parameters {
		/**
		 * @brief Column header widgets.
		 * These widgets will be put inside of a horizontal ruis::tiling_area.
		 */
		ruis::widget_list column_headers = {};
		utki::shared_ref<table_tree_view::provider> provider;
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

	using table_list::set_scroll_factor;
	using table_list::get_scroll_factor;
	using table_list::get_scroll_band;
	using table_list::get_pos_index;
	using table_list::get_pos_offset;
	using table_list::scroll_by;
	using table_list::model_change_handler;
	using table_list::scroll_change_handler;
};

namespace make {
utki::shared_ref<ruis::table_tree_view> table_tree_view(
	utki::shared_ref<ruis::context> context, //
	ruis::table_tree_view::all_parameters params
);
} // namespace make

} // namespace ruis
