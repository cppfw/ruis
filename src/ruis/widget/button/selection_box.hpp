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

#include "../base/list_widget.hpp"
#include "../container.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Drop down box base class.
 * This is a base class for drop down box widgets.
 * Drop down box widget allows selection of an item from list of items.
 */
class selection_box :
	virtual public widget, //
	public list_widget
{
	// TODO: use shared_ref
	// NOLINTNEXTLINE(clang-analyzer-webkit.NoUncountedMemberChecker, "false-positivie")
	container& selection_container;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		list_widget::parameters list_params;
	};

private:
	size_t selected_index = 0;

protected:
	selection_box(
		utki::shared_ref<ruis::context> context, //
		container& selection_container,
		list_widget::parameters list_params
	);

public:
	selection_box(const selection_box&) = delete;
	selection_box& operator=(const selection_box&) = delete;

	selection_box(selection_box&&) = delete;
	selection_box& operator=(selection_box&&) = delete;

	~selection_box() override = default;

	/**
	 * @brief Set currently selected item.
	 * @param i - index of the item to set as currently selected.
	 */
	void set_selection(size_t i);

	/**
	 * @brief Get index of the selected item.
	 * @return Index of the selected item.
	 */
	size_t get_selection() const noexcept
	{
		return this->selected_index;
	}

	std::function<void(selection_box& sb)> selection_handler;

	void on_reload() override;

private:
	void handle_model_change() override;
};

namespace make {
utki::shared_ref<ruis::selection_box> selection_box(
	utki::shared_ref<ruis::context> context, //
	ruis::selection_box::all_parameters params
);
} // namespace make

} // namespace ruis
