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

#include "list.hpp"
#include "tiling_area.hpp"

namespace ruis {

namespace internal {
class provider;
} // namespace internal

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::enable_shared_from_this is public via widget inheritance")
class table_list :
	virtual public widget, //
	private container
{
	friend class ruis::internal::provider;

public:
	/**
	 * @brief A class for accessing the tree data model.
	 * This class provides access to the tree data model and constructs
	 * widgets representing the tree items.
	 */
	class provider
	{
	public:
		const utki::shared_ref<ruis::context> context;

		provider(utki::shared_ref<ruis::context> context) :
			context(std::move(context))
		{}

		provider(const provider&) = delete;
		provider& operator=(const provider&) = delete;

		provider(provider&&) = delete;
		provider& operator=(provider&&) = delete;

		virtual ~provider() = default;

		virtual size_t count() const noexcept = 0;

		/**
		 * @brief Create table row cell widgets for an item.
		 * The table_list will call this mthod when it needs widgets for the given list data item.
		 * A list of widgets is to be created.
		 * Each child widget will represent a cell in the table row.
		 * @param index - index of the item into the list data to create widgets for.
		 * @return A list of widgets for table row cell.
		 */
		virtual widget_list get_row_widgets(size_t index) = 0;
	};

	struct parameters {
		/**
		 * @brief Column header widgets.
		 * These widgets will be put inside of a horizontal ruis::tiling_area.
		 */
		ruis::widget_list column_headers = {};
		utki::shared_ref<table_list::provider> provider;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
		parameters table_list_params;
	};

	table_list(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

private:
	table_list(
		utki::shared_ref<tiling_area> ta, //
		utki::shared_ref<list> l,
		all_parameters& params
	);

	void arrange_list_item_cells(ruis::semiconst_widget_list& cells);

	utki::shared_ref<tiling_area> headers_tiling_area;
	utki::shared_ref<list> table_rows_list;
};

namespace make {
utki::shared_ref<ruis::table_list> table_list(
	utki::shared_ref<ruis::context> context, //
	ruis::table_list::all_parameters params
);
} // namespace make

} // namespace ruis
