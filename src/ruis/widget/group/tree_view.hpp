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
#include "scroll_area.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class tree_view :
	virtual public widget, //
	private scroll_area
{
	utki::shared_ref<list> item_list;

public:
	/**
	 * @brief tree_view item provider base class.
	 * User subclasses this class to provide tree_view an access to the tree data model
	 * and provide a way to represent the data as widgets.
	 */
	class provider : private list_provider
	{
		friend class tree_view;

		void recycle(size_t index, utki::shared_ref<widget> w) override;

		utki::shared_ref<widget> get_widget(size_t index) override;

		size_t count() const noexcept override;

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

		const decltype(iter)& iter_for(size_t index) const;

		void remove_children(decltype(iter) from);
		void set_children(decltype(iter) i, size_t num_children);

	public:
		/**
		 * @brief Get stored ruis context.
		 * @return The stored ruis context.
		 */
		const utki::shared_ref<ruis::context>& get_context() noexcept
		{
			return this->context;
		}

		/**
		 * @brief Construct tree_view items provider.
		 * @param context - ruis context to store. The context can be obtained using the get_context() member function.
		 */
		provider(utki::shared_ref<ruis::context> context) :
			list_provider(std::move(context))
		{}

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
		void on_reload() override {}

		void expand(utki::span<const size_t> index);
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
		std::shared_ptr<tree_view::provider> provider;
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
	 * @brief Scroll position changed signal.
	 * Emitted when tree_view's vertical or horizontal scroll position has changed.
	 */
	std::function<void(tree_view&)> scroll_change_handler;

	/**
	 * @brief Set items provider.
	 * @param provider - items provider.
	 */
	void set_provider(std::shared_ptr<provider> provider = nullptr);

	/**
	 * @brief Set vertical scroll position by factor.
	 * @param factor - new scroll position specified by factor from [0:1].
	 */
	void set_vertical_scroll_factor(real factor)
	{
		this->item_list.get().set_scroll_factor(factor);
	}

	/**
	 * @brief Set horizontal scroll position by factor.
	 * @param factor - new scroll position specified by factor from [0:1].
	 */
	void set_horizontal_scroll_factor(real factor)
	{
		this->set_scroll_factor(vector2(factor, 0));
	}

	/**
	 * @brief Get scroll position.
	 * @return Vector of (horizontal scroll factor, vertical scroll factor).
	 */
	vector2 get_scroll_factor() const
	{
		return vector2(
			this->scroll_area::get_scroll_factor().x(), //
			this->item_list.get().get_scroll_factor()
		);
	}

	/**
	 * @brief Get scroll band.
	 * Get scroll band size.
	 * The scroll band size is a fraction of [0:1] interval.
	 * @return Vector of (horizontal scroll band, vertical scroll band).
	 */
	vector2 get_scroll_band() const noexcept
	{
		return vector2(
			this->scroll_area::get_visible_area_fraction().x(), //
			this->item_list.get().get_scroll_band()
		);
	}

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
