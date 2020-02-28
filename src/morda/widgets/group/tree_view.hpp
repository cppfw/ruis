#pragma once

#include <memory>

#include <utki/tree.hpp>

#include "../widget.hpp"
#include "list.hpp"

#include "scroll_area.hpp"

namespace morda{

class tree_view :
		virtual public widget,
		private scroll_area
{
	std::shared_ptr<list_widget> item_list;
public:
	tree_view(std::shared_ptr<morda::context> c, const puu::forest& desc);

	tree_view(const tree_view&) = delete;
	tree_view& operator=(const tree_view&) = delete;

	class provider :
			public virtual utki::shared,
			private list_widget::provider
	{
		friend class tree_view;
		friend std::shared_ptr<list_widget::provider> std::static_pointer_cast<list_widget::provider>(const std::shared_ptr<provider>&)noexcept;

		void recycle(size_t index, std::shared_ptr<widget> w)override;

		std::shared_ptr<widget> get_widget(size_t index)override;

		size_t count()const noexcept override;

		struct node{
			size_t subtree_size = 0;
		};
		
		mutable utki::tree<node> visible_tree;

		utki::traversal<decltype(visible_tree.children)> traversal()const noexcept{
			return utki::make_traversal(this->visible_tree.children);
		}

		// cached values for faster lookup by index
		mutable size_t iter_index = 0;
		mutable utki::traversal<decltype(visible_tree)::container_type>::iterator iter = this->traversal().begin();

		const decltype(iter)& iter_for(size_t index)const;

		void remove_children(decltype(iter) from);
		void set_children(decltype(iter) i, size_t num_children);

	protected:
		provider(){
			this->notify_data_set_changed();
		}
	public:

		virtual std::shared_ptr<widget> get_widget(const std::vector<size_t>& index, bool isCollapsed) = 0;

		virtual void recycle(const std::vector<size_t>& index, std::shared_ptr<widget> w){}

		virtual size_t count(const std::vector<size_t>& index)const noexcept = 0;

		void uncollapse(const std::vector<size_t>& index);
		void collapse(const std::vector<size_t>& index);

		void notify_data_set_changed();

		void notify_item_changed(){
			this->list_widget::provider::notify_data_set_changed();
		}

		/**
		 * @brief Notify that an item has been removed.
		 * @param index - index path of the removed item.
		 */
		void notify_item_removed(const std::vector<size_t>& index);

		/**
		 * @brief Notify that a new item has been added.
		 * @param index - index path to a newly added item. Essentially, it is a path
		 *                to an item before which a new item has been added.
		 */
		void notify_item_added(const std::vector<size_t>& index);

	private:

	};

public:
	/**
	 * @brief Invoked when view Changes.
	 * For example on collapse/uncollapse.
	 */
	std::function<void(tree_view&)> view_change_handler;

	void set_provider(std::shared_ptr<provider> provider = nullptr);

	void set_vertical_scroll_factor(real factor){
		this->item_list->set_scroll_factor(factor);
	}

	void set_horizontal_scroll_factor(real factor){
		this->set_scroll_factor(vector2(factor, 0));
	}

	vector2 get_scroll_factor()const{
		return vector2(this->scroll_area::get_scroll_factor().x, this->item_list->get_scroll_factor());
	}
};

}
