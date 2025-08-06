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

#include <memory>

#include <utki/tree.hpp>

#include "../widget.hpp"

#include "list.hpp"

namespace ruis {

namespace internal {
class tree_view_list_provider;
} // namespace internal

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class tree_view :
	virtual public widget, //
	private list
{
public:
	/**
	 * @brief tree_view item provider base class.
	 * User subclasses this class to provide tree_view an access to the tree data model
	 * and provide a way to represent the data as widgets.
	 */
	class provider
	{
		friend class internal::tree_view_list_provider;

		ruis::list_provider* list_provider = nullptr;

		struct node {
			size_t subtree_size = 0; // size of the visible subtree
		};

		mutable utki::tree<node> visible_tree;

		utki::traversal<decltype(visible_tree.children)> traversal() const noexcept
		{
			return utki::make_traversal(this->visible_tree.children);
		}

		// cached values for faster lookup by index
		mutable size_t iter_index = 0;
		mutable utki::traversal<decltype(visible_tree)::container_type>::iterator iter = this->traversal().begin();

		void init();

		const decltype(iter)& iter_for(size_t index) const;

		void remove_children(decltype(iter) from);
		void set_children(
			decltype(iter) i, //
			size_t num_children
		);

		size_t list_count() const noexcept;

		void list_recycle(
			size_t index, //
			utki::shared_ref<widget> w
		);

		utki::shared_ref<widget> list_get_widget(size_t index);

	public:
		const utki::shared_ref<ruis::context> context;

		/**
		 * @brief Construct tree_view items provider.
		 * @param context - ruis context to store.
		 */
		provider(utki::shared_ref<ruis::context> context);

		provider(const provider&) = delete;
		provider& operator=(const provider&) = delete;

		provider(provider&&) = delete;
		provider& operator=(provider&&) = delete;

		virtual ~provider() = default;

		/**
		 * @brief Create item widget.
		 * The tree_view will call this function when it needs an item widget for the given index.
		 * @param index - index into the data model to create an item widget for.
		 * @param prefix_widgets - a list of tree_view specific widgets, i.e. colapse/expande button and path indicators.
		 *                         To be placed inside of a row.
		 * @return The item widget.
		 */
		virtual utki::shared_ref<widget> get_widget(
			utki::span<const size_t> index, //
			widget_list prefix_widgets
		);

		/**
		 * @brief Create item widget.
		 * This function is called by the get_widghet(index, prefix_widgets) overload.
		 * @param index - index into the data model to create an item widget for.
		 * @return The item widget.
		 */
		virtual utki::shared_ref<widget> get_widget(utki::span<const size_t> index) = 0;

		/**
		 * @brief Recycle item widget.
		 * The tree_view will call this function when it no longer needs the item widget.
		 * This happens when the item widget goes beyond the tree_view bounds due to scrolling.
		 * @param index - index into the data model for which the widget is to be recycled.
		 * @param w - the widget to be recycled.
		 */
		virtual void recycle(
			utki::span<const size_t> index, //
			const utki::shared_ref<widget>& w
		)
		{}

		/**
		 * @brief Get number of tree node's children.
		 * The tree_view will call this function when it needs to know the number of
		 * children of a tree node.
		 * @param index - index of the tree node to gen number of children for.
		 */
		virtual size_t count(utki::span<const size_t> index) const noexcept = 0;

		/**
		 * @brief Reload callback.
		 * Called by owner tree_view's on_reload().
		 */
		virtual void on_reload() {}

		/**
		 * @brief Expand item.
		 * Expands the tree item if it has children.
		 * @param index - index of the item to expand.
		 */
		void expand(utki::span<const size_t> index);

		/**
		 * @brief Collapse item.
		 * Collapse tree item if it was expanded.
		 * @param index - index of the item to collapse.
		 */
		void collapse(utki::span<const size_t> index);

		/**
		 * @brief Notify about any model change.
		 * Calling this function will cause the tree_view to re-create and
		 * re-layout it's contents.
		 */
		void notify_model_change();

		/**
		 * @brief Notify about tree item contents change.
		 * Use this function to notify about changes to the tree data which do not involve
		 * adding new items or removing items. I.e. when tree topology does not chnage.
		 * Calling this function will cause the tree_view to re-create and
		 * re-layout it's contents.
		 * This operation should be faster than notify_model_change().
		 */
		void notify_item_change();

		/**
		 * @brief Notify that an item has been removed.
		 * @param index - index path of the removed item.
		 */
		void notify_item_remove(utki::span<const size_t> index);

		/**
		 * @brief Notify that a new item has been added.
		 * @param index - index path to a newly added item. Essentially, it is a path
		 *                to an item before which a new item has been added.
		 */
		void notify_item_add(utki::span<const size_t> index);
	};

	struct parameters {
		// TODO: make utki::unique_ref
		utki::shared_ref<tree_view::provider> provider;
	};

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		parameters tree_view_params;
	};

	tree_view(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	tree_view(const tree_view&) = delete;
	tree_view& operator=(const tree_view&) = delete;

	tree_view(tree_view&&) = delete;
	tree_view& operator=(tree_view&&) = delete;

	~tree_view() override = default;

	/**
	 * @brief Scroll position changed handler.
	 * Invoked when tree_view's scroll position has changed.
	 */
	std::function<void(tree_view&)> scroll_change_handler;

	/**
	 * @brief Set vertical scroll position by factor.
	 * @param factor - new scroll position specified by factor from [0:1].
	 * @param notify_scroll_change - whether to invoke the scroll change handler.
	 */
	using list::set_scroll_factor;

	/**
	 * @brief Get scroll position.
	 * @return Scroll factor.
	 */
	using list::get_scroll_factor;

	/**
	 * @brief Get scroll band.
	 * Get scroll band size.
	 * The scroll band size is a fraction of [0:1] interval.
	 * @return Vertical scroll band.
	 */
	using list::get_scroll_band;

private:
	void notify_view_change();
};

namespace make {
inline utki::shared_ref<ruis::tree_view> tree_view(
	utki::shared_ref<ruis::context> context, //
	ruis::tree_view::all_parameters params
)
{
	return utki::make_shared<ruis::tree_view>(
		std::move(context), //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
