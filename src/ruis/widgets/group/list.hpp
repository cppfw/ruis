/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../../container.hpp"
#include "../../util/oriented.hpp"
#include "../../widget.hpp"

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
	public container,
	protected oriented
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

protected:
	list(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc, bool vertical);

public:
	list(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
		list(c, desc, true)
	{}

	list(const list&) = delete;
	list& operator=(const list&) = delete;

	list(list&&) = delete;
	list& operator=(list&&) = delete;

	~list() override = default;

	/**
	 * @brief list items provider.
	 * User should subclass this class to provide items to the list.
	 */
	class provider : virtual public utki::shared
	{
		friend class list;

		list* parent_list = nullptr;

	protected:
		provider() = default;

	public:
		provider(const provider&) = delete;
		provider& operator=(const provider&) = delete;

		provider(provider&&) = delete;
		provider& operator=(provider&&) = delete;

		~provider() override = default;

		/**
		 * @brief Get parent list widget.
		 * @return list widget which owns the provider, in case the provider is set to some list widget.
		 * @return nullptr in case the provider is not set to any list widget.
		 */
		list* get_list() noexcept
		{
			return this->parent_list;
		}

		/**
		 * @brief Get total number of items in the list.
		 * @return Number of items in the list.
		 */
		virtual size_t count() const noexcept = 0;

		/**
		 * @brief Get widget for item.
		 * @param index - index of item to get widget for.
		 * @return widget for the requested item.
		 */
		virtual utki::shared_ref<widget> get_widget(size_t index) = 0;

		/**
		 * @brief Recycle widget of item.
		 * @param index - index of item to recycle widget of.
		 * @param w - widget to recycle.
		 */
		virtual void recycle(size_t index, const utki::shared_ref<widget>& w) {}

		void notify_data_set_change();
	};

	void set_provider(std::shared_ptr<provider> item_provider = nullptr);

	void on_lay_out() override;

	ruis::vector2 measure(const ruis::vector2& quotum) const override;

	/**
	 * @brief Set scroll position as factor from [0:1].
	 * @param factor - factor of the scroll position to set.
	 */
	void set_scroll_factor(real factor);

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
	 * @brief Data set changed signal.
	 * Emitted when list widget contents have actually been updated due to change in provider's model data set.
	 */
	std::function<void(list&)> data_set_change_handler;

	/**
	 * @brief Scroll position changed signal.
	 * Emitted when list's scroll position has changed.
	 */
	std::function<void(list&)> scroll_change_handler;

private:
	std::shared_ptr<provider> item_provider;

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

	void handle_data_set_changed();

	void notify_scroll_pos_changed();
	void notify_scroll_pos_changed(size_t old_index, real old_offset);

	real calc_num_visible_items() const noexcept;
};

/**
 * @brief Horizontal list widget.
 * Panorama list widget.
 * From GUI script it can be instantiated as "pan_list".
 */
class pan_list : public list
{
public:
	pan_list(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
		widget(c, desc),
		list(this->context, desc, false)
	{}

	pan_list(const pan_list&) = delete;
	pan_list& operator=(const pan_list&) = delete;

	pan_list(pan_list&&) = delete;
	pan_list& operator=(pan_list&&) = delete;

	~pan_list() override = default;
};

} // namespace ruis
