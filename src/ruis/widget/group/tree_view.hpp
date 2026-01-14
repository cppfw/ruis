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

#include <memory>

#include <utki/tree.hpp>

#include "../widget.hpp"

#include "list.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class tree_view :
	virtual public widget, //
	private list
{
	class list_provider_for_tree_view;

public:
	class provider_base
	{
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

		void set_iter_to(size_t index) const;

		void remove_children(decltype(iter) from);
		void set_children(
			decltype(iter) i, //
			size_t num_children
		);

	protected:
		size_t list_count() const noexcept;

		virtual void on_list_model_changed() = 0;

		void init();

		struct tree_item_widget_parts {
			ruis::widget_list prefix_widgets;
			std::vector<size_t> index;
		};

		tree_item_widget_parts get_item_widget_parts(size_t index);

	public:
		const utki::shared_ref<ruis::context> context;

		provider_base(utki::shared_ref<ruis::context> context) :
			context(std::move(context))
		{}

		provider_base(const provider_base&) = delete;
		provider_base& operator=(const provider_base&) = delete;

		provider_base(provider_base&&) = delete;
		provider_base& operator=(provider_base&&) = delete;

		virtual ~provider_base() = default;

		/**
		 * @brief Get number of tree node's children.
		 * The tree_view will call this function when it needs to know the number of
		 * children of a tree node.
		 * @param index - index of the tree node to gen number of children for.
		 */
		virtual size_t count(utki::span<const size_t> index) const noexcept = 0;

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
		 * @brief Notify that an item has been removed.
		 * @param index - index path of the removed item.
		 */
		void notify_item_removed(utki::span<const size_t> index);

		/**
		 * @brief Notify that a new item has been added.
		 * @param index - index path to a newly added item. Essentially, it is a path
		 *                to an item before which a new item has been added.
		 */
		void notify_item_added(utki::span<const size_t> index);

		/**
		 * @brief Notify about tree item contents change.
		 * Use this function to notify about changes to the tree data which do not involve
		 * adding new items or removing items. I.e. when tree topology does not chnage.
		 * Calling this function will cause the tree_view to re-create and
		 * re-layout it's contents.
		 * This operation should be faster than notify_model_change().
		 */
		void notify_item_changed();

		/**
		 * @brief Notify about any model change.
		 * Calling this function will cause the tree_view to re-create and
		 * re-layout it's contents.
		 */
		void notify_model_changed();
	};

	/**
	 * @brief tree_view item provider base class.
	 * User subclasses this class to provide tree_view an access to the tree data model
	 * and provide a way to represent the data as widgets.
	 */
	class provider : public provider_base
	{
		friend class list_provider_for_tree_view;

		ruis::list_provider* list_provider = nullptr;

		utki::shared_ref<widget> list_get_widget(size_t index);

	public:
		/**
		 * @brief Construct tree_view items provider.
		 * @param context - ruis context to store.
		 */
		provider(utki::shared_ref<ruis::context> context);

		provider(const provider&) = delete;
		provider& operator=(const provider&) = delete;

		provider(provider&&) = delete;
		provider& operator=(provider&&) = delete;

		~provider() override = default;

		/**
		 * @brief Create item widget.
		 * This function is called by the provider_base::get_widget(index, prefix_widgets).
		 * @param index - index into the data model to create an item widget for.
		 * @return The item widget.
		 */
		virtual utki::shared_ref<widget> get_widget(utki::span<const size_t> index) = 0;

	protected:
		void on_list_model_changed() override;
	};

	struct parameters {
		utki::shared_ref<tree_view::provider> provider;
	};

	struct all_parameters {
		layout::parameters layout_params;
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
