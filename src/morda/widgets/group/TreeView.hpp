#pragma once

#include <memory>

#include <utki/tree.hpp>

#include "../widget.hpp"
#include "List.hpp"

#include "ScrollArea.hpp"


namespace morda{

class TreeView :
		virtual public Widget,
		private ScrollArea
{
	std::shared_ptr<List> list;
public:
	TreeView(std::shared_ptr<morda::context> c, const puu::forest& desc);

	TreeView(const TreeView&) = delete;
	TreeView& operator=(const TreeView&) = delete;


	class ItemsProvider :
			public virtual utki::shared,
			private List::ItemsProvider
	{
		friend class TreeView;
		friend std::shared_ptr<List::ItemsProvider> std::static_pointer_cast<List::ItemsProvider>(const std::shared_ptr<ItemsProvider>&)noexcept;

		void recycle(size_t index, std::shared_ptr<Widget> w)override;

		std::shared_ptr<Widget> getWidget(size_t index)override;

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
		ItemsProvider(){
			this->notifyDataSetChanged();
		}
	public:

		virtual std::shared_ptr<widget> getWidget(const std::vector<size_t>& index, bool isCollapsed) = 0;

		virtual void recycle(const std::vector<size_t>& index, std::shared_ptr<widget> w){}

		virtual size_t count(const std::vector<size_t>& index)const noexcept = 0;

		void uncollapse(const std::vector<size_t>& index);
		void collapse(const std::vector<size_t>& index);

		void notifyDataSetChanged();

		void notifyItemChanged(){
			this->List::ItemsProvider::notifyDataSetChanged();
		}

		/**
		 * @brief Notify that an item has been removed.
		 * @param index - index path of the removed item.
		 */
		void notifyItemRemoved(const std::vector<size_t>& index);

		/**
		 * @brief Notify that a new item has been added.
		 * @param index - index path to a newly added item. Essentially, it is a path
		 *                to an item before which a new item has been added.
		 */
		void notifyItemAdded(const std::vector<size_t>& index);

	private:

	};

public:
	/**
	 * @brief Invoked when view Changes.
	 * For example on collapse/uncollapse.
	 */
	std::function<void(TreeView&)> viewChanged;

	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);

	void setVerticalScrollPosAsFactor(real factor){
		this->list->setScrollPosAsFactor(factor);
	}

	void setHorizontalScrollPosAsFactor(real factor){
		this->setScrollPosAsFactor(Vec2r(factor, 0));
	}

	Vec2r scrollFactor()const{
		return Vec2r(this->ScrollArea::scrollFactor().x, this->list->scrollFactor());
	}
};

}
