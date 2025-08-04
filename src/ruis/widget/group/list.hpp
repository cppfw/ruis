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

#include "../../util/oriented.hpp"
#include "../base/list_widget.hpp"
#include "../container.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Scrollable list widget.
 * This is a base class for vertical and horizontal lists.
 */
class list :
	// NOTE: order of virtual public and private declarations here matters for clang due to some bug,
	//       see
	//       http://stackoverflow.com/questions/42427145/clang-cannot-cast-to-private-base-while-there-is-a-public-virtual-inheritance
	// UPDATE 2025-07-29: private container inheritance changed to be public.
	virtual public widget,
	// inherit container publicly because sometimes it is needed to access current visible child widgets of the list
	public container,
	public oriented,
	public list_widget
{
	// index of the first item added to container as child
	size_t added_index = size_t(-1);

	size_t pos_index = 0; // index of the first visible item

	// offset in pixels of the first visible item.
	// the value is positive, tough the item is biased towards negative coordinate values.
	real pos_offset = real(0);

	size_t num_tail_items = 0; // zero means that number of tail items has to be recomputed
	size_t first_tail_item_index = 0;
	real first_tail_item_offset = real(0);
	real first_tail_item_dim = real(0);

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		oriented::parameters oriented_params;
		list_widget::parameters list_params;
	};

	list(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	list(const list&) = delete;
	list& operator=(const list&) = delete;

	list(list&&) = delete;
	list& operator=(list&&) = delete;

	~list() override = default;

	void on_lay_out() override;

	ruis::vector2 measure(const ruis::vector2& quotum) const override;

	void on_reload() override;

	/**
	 * @brief Set scroll position as factor from [0:1].
	 * @param factor - factor of the scroll position to set.
	 * @param notify_change - whether to invoke the scroll change handler.
	 */
	void set_scroll_factor(
		real factor, //
		bool notify_scroll_change = true
	);

	/**
	 * @brief Get scroll factor.
	 * @return Current scroll position as factor from [0:1].
	 */
	real get_scroll_factor() const noexcept;

	/**
	 * @brief Get scroll band.
	 * Returns scroll band as a fraction of 1. This is basically the number of visible elements divided by total number
	 * of elements in the list.
	 * @return scroll band.
	 */
	real get_scroll_band() const noexcept;

	/**
	 * @brief Get index of the first visible item.
	 * @return index of the first visible item.
	 */
	size_t get_pos_index() const noexcept
	{
		return this->pos_index;
	}

	/**
	 * @brief Get offset of the first visible item.
	 * The value is positive, though the item coordinate is <= 0.
	 * @return offset in pixels of the first visible item.
	 */
	real get_pos_offset() const noexcept
	{
		return this->pos_offset;
	}

	/**
	 * @brief Scroll the list by given number of pixels.
	 * @param delta - number of pixels to scroll, can be positive or negative.
	 */
	void scroll_by(real delta);

	/**
	 * @brief Model change signal.
	 * Emitted when list widget contents have actually been updated due to change in provider's model data set.
	 */
	std::function<void(list&)> model_change_handler;

	/**
	 * @brief Scroll position changed signal.
	 * Emitted when list's scroll position has changed.
	 */
	std::function<void(list&)> scroll_change_handler;

private:
	void update_children_list();

	// returns true if it was the last visible widget
	bool arrange_widget(
		const utki::shared_ref<widget>& w,
		real& pos,
		bool add,
		size_t index,
		widget_list::const_iterator& insert_before
	);

	void update_tail_items_info();

	void handle_model_change() override;

	void notify_scroll_pos_changed();
	void notify_scroll_pos_changed(size_t old_index, real old_offset);

	real calc_num_visible_items() const noexcept;
};

namespace make {
inline utki::shared_ref<ruis::list> list( //
	utki::shared_ref<ruis::context> context,
	list::all_parameters params
)
{
	return utki::make_shared<ruis::list>( //
		std::move(context),
		std::move(params)
	);
}

} // namespace make

} // namespace ruis
