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
	virtual public widget,
	private container,
	public oriented,
	public list_widget
{
	// index of the first item added to container as child
	size_t added_index = size_t(-1);

	// index of the first visible item
	size_t pos_index = 0;

	// Offset in pixels of the first visible item.
	// The value is positive, tough the item is biased towards negative coordinate values.
	real pos_offset = real(0);

	// zero means that number of tail items has to be recomputed
	size_t num_tail_items = 0;

	size_t first_tail_item_index = 0;
	real first_tail_item_offset = real(0);
	real first_tail_item_dim = real(0);

public:
	/**
	 * @brief List widget specific parameters.
	 */
	struct parameters {
		/**
		 * @brief Oriented widget parameters.
		 */
		ruis::oriented::parameters oriented;

		/**
		 * @brief List widget parameters, like item widgets provider.
		 */
		ruis::list_widget::parameters specific;
	};

	/**
	 * @brief All list widget parameters.
	 */
	struct all_parameters {
		/**
		 * @brief Layout parameters.
		 */
		ruis::layout_parameters layout_params;

		/**
		 * @brief Base widget parameters.
		 */
		ruis::widget::parameters widget;

		/**
		 * @brief List widget specific parameters.
		 */
		parameters params;
	};

	// default clip parameter is true
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - list widget parameters.
	 */
	list(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	list(const list&) = delete;
	list& operator=(const list&) = delete;

	list(list&&) = delete;
	list& operator=(list&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	~list() override = default;

	/**
	 * @brief Invoked when the list needs to lay out itself.
	 */
	void on_lay_out() override;

	/**
	 * @brief Measure how big the list wants to be.
	 * @param quotum - space available to the list.
	 * @param mode - measurement mode for each dimension.
	 * @return Measured desired list dimensions.
	 */
	ruis::vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const override;

	/**
	 * @brief Set scroll position as factor from [0:1].
	 * @param factor - factor of the scroll position to set.
	 * @param notify_scroll_change - whether to invoke the scroll change handler.
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
	 * @return number of pixels actually scrolled by.
	 */
	real scroll_by(real delta);

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

	/**
	 * @brief Get visible children widgets.
	 * The returned span is only valid in local scope. The list manages visible children widgets itself,
	 * so the span may change after list layout or other operations modifying the list's contents.
	 * @return span of visible children widgets.
	 */
	utki::span<const utki::shared_ref<widget>> get_visible_children() noexcept
	{
		return this->children();
	}

protected:
	using container::on_mouse_move;
	using container::on_mouse_button;

private:
	/**
	 * @brief Update the list of visible children widgets.
	 */
	void update_children_list();

	/**
	 * @brief Arrange a single widget in the list.
	 * @param w - widget to arrange.
	 * @param pos - current position along the list, updated as a result.
	 * @param add - true to add the widget to the visible children list, false to remove it.
	 * @param index - index of the widget in the list.
	 * @param insert_before - iterator in the visible children list before which the widget should be inserted.
	 * @return true if it was the last visible widget.
	 */
	bool arrange_widget(
		const utki::shared_ref<widget>& w,
		real& pos,
		bool add,
		size_t index,
		widget_list::const_iterator& insert_before
	);

	/**
	 * @brief Update the tail items info, i.e. the total size of the items
	 * that follow the visible items and become visible when the list
	 * is scrolled to the bottom.
	 */
	void update_tail_items_info();

	/**
	 * @brief Invoked when the list's model data set has changed.
	 */
	void handle_model_change() override;

	/**
	 * @brief Notify that the scroll position has changed.
	 */
	void notify_scroll_pos_changed();

	/**
	 * @brief Notify that the scroll position has changed.
	 * @param old_index - index of the first visible item before the change.
	 * @param old_offset - offset of the first visible item before the change.
	 */
	void notify_scroll_pos_changed(size_t old_index, real old_offset);

	/**
	 * @brief Calculate the number of visible items for the current scroll position.
	 * The count may be fractional because of partially visible items.
	 * @return number of items that fit in the visible area of the list.
	 */
	real calc_num_visible_items() const noexcept;
};

namespace make {
/**
 * @brief Construct a list widget.
 * @param context - ruis context.
 * @param params - list widget parameters.
 * @return newly constructed list widget.
 */
inline utki::shared_ref<ruis::list> list( //
	const utki::shared_ref<ruis::context>& context,
	list::all_parameters params
)
{
	return utki::make_shared<ruis::list>( //
		context,
		std::move(params)
	);
}

} // namespace make

} // namespace ruis
