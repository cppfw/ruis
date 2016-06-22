/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <memory>

#include "core/Widget.hpp"
#include "List.hpp"

#include "core/container/ScrollContainer.hpp"


namespace morda{


class Tree{
	size_t size_var = 0;
	
	std::vector<Tree> children;
	
public:
	class Iterator{
		friend class Tree;
		
		std::vector<size_t> pathIdx;
		std::vector<Tree*> pathPtr;
		
		Iterator(Tree* node, size_t idx){
			ASSERT(node)
			this->pathPtr.push_back(node);
			this->pathIdx.push_back(idx);
		}
	public:
		Iterator() = default;
		
		Iterator(const Iterator&) = default;
		Iterator& operator=(const Iterator&) = default;
		
		size_t depth()const noexcept{
			return this->pathIdx.size();
		}
		
		const decltype(pathIdx)& path()const noexcept{
			return this->pathIdx;
		}
		
		Tree& parent(){
			ASSERT(this->pathPtr.size() != 0)
			return *this->pathPtr.back();
		}
		
		Tree& operator*(){
			ASSERT(this->pathPtr.size() != 0)
			ASSERT(this->pathIdx.size() == this->pathPtr.size())
			ASSERT(this->pathIdx.back() < this->pathPtr.back()->children.size())
			return this->pathPtr.back()->children[this->pathIdx.back()];
		}
		
		Iterator& operator++(){
			if(this->pathIdx.size() == 0){
				return *this;
			}
			
			{
				auto& list = this->pathPtr.back()->children;
				auto& idx = this->pathIdx.back();

				if(list[idx].children.size() != 0){
					this->pathPtr.push_back(&list[idx]);
					this->pathIdx.push_back(0);
					return *this;
				}
			}

			for(; this->pathIdx.size() != 0;){
				auto& list = this->pathPtr.back()->children;
				auto& idx = this->pathIdx.back();
				
				++idx;
				
				if(idx == list.size()){
					this->pathPtr.pop_back();
					this->pathIdx.pop_back();
				}else{
					break;
				}
			}
			
			return *this;
		}
		
		Iterator& moveToNext(){
			if(!this->operator bool()){
				return *this;
			}
			ASSERT(this->pathIdx.size() != 0)
			ASSERT(this->pathPtr.size() == this->pathIdx.size())
			++this->pathIdx.back();
			return *this;
		}
		
		Iterator& moveToPrev(){
			if(!this->operator bool()){
				return *this;
			}
			ASSERT(this->pathIdx.size() != 0)
			ASSERT(this->pathPtr.size() == this->pathIdx.size())
			--this->pathIdx.back();
			return *this;
		}
		
		Iterator operator+(size_t i)const{
			return Iterator(*this) += i;
		}
		
		Iterator& operator--(){
			if(this->pathIdx.size() == 0){
				return *this;
			}
			
			if(this->pathIdx.back() == 0){
				this->pathPtr.pop_back();
				this->pathIdx.pop_back();
				return *this;
			}
						
			for(;;){
				auto& list = this->pathPtr.back()->children;
				auto& idx = this->pathIdx.back();
				ASSERT(idx != 0)
				--idx;
				
				if(list[idx].children.size() != 0){
					this->pathPtr.push_back(&list[idx]);
					this->pathIdx.push_back(list[idx].children.size());
				}else{
					break;
				}
			}
			
			return *this;
		}
		
		
		
		Iterator& operator+=(size_t d){
			for(decltype(d) i = 0; i != d && this->pathIdx.size() != 0; ++i){
				this->operator++();
			}
			return *this;
		}
		
		Iterator& operator-=(size_t d){
			for(decltype(d) i = 0; i != d && this->pathIdx.size() != 0; ++i){
				this->operator--();
			}
			return *this;
		}
		
		bool operator==(const Iterator& i)const{
			return this->pathIdx == i.pathIdx;
		}
		
		bool operator!=(const Iterator& i)const{
			return !this->operator==(i);
		}
		
		bool operator>(const Iterator& i)const{
			return this->pathIdx > i.pathIdx;
		}
		
		bool operator<(const Iterator& i)const{
			return this->pathIdx < i.pathIdx;
		}
		
		bool operator>=(const Iterator& i)const{
			return this->operator>(i) || this->operator ==(i);
		}
		
		bool operator<=(const Iterator& i)const{
			return this->operator<(i) || this->operator ==(i);
		}
		
		explicit operator bool()const{
			return this->path().size() != 0;
		}
		
		Iterator& ascent(){
			this->pathIdx.pop_back();
			this->pathPtr.pop_back();
			return *this;
		}
		
		Iterator& descent(size_t index){
			if(this->pathIdx.size() == 0){
				return *this;
			}
			
			if(this->pathIdx.back() >= this->pathPtr.back()->children.size()){
				return *this;
			}
			this->pathPtr.push_back(&this->operator*());
			this->pathIdx.push_back(index);
			return *this;
		}
	};
	
	void correctIteratorAfterDeletion(Iterator& iter, const std::vector<size_t>& deletedPath){
		auto i = iter.pathIdx.begin();
		auto j = deletedPath.begin();
		for(; i != iter.pathIdx.end() && j != deletedPath.end(); ++i, ++j){
			if(*i != *j){
				if(j != deletedPath.end() - 1){
					break;//items are in different branches, no correction is needed
				}

				if(*i > *j){
					--(*i);
				}
				break;
			}else{
				if(j == deletedPath.end() - 1){
					iter.pathIdx = deletedPath;
					break;
				}
			}
		}
		iter.pathPtr = this->pos(iter.path()).pathPtr;
		ASSERT(iter.pathPtr.size() != 0 && iter.pathIdx.size() == iter.pathPtr.size())
		ASSERT_INFO(iter.pathPtr.back()->numChildren() >= iter.pathIdx.back(), "given iterator was malformed")
		while(iter.path().size() != 0 && iter.pathPtr.back()->numChildren() == iter.pathIdx.back()){
			iter.ascent();
			if(iter.pathIdx.size() != 0){
				++iter.pathIdx.back();
			}
		}
	}
	
	void correctIteratorAfterAddition(Iterator& iter, const std::vector<size_t>& addedPath){
		auto i = iter.pathIdx.begin();
		auto j = addedPath.begin();
		for(; i != iter.pathIdx.end() && j != addedPath.end(); ++i, ++j){
			if(*i != *j){
				if(j != addedPath.end() - 1){
					break;//items are in different branches, no correction needed
				}
				
				if(*i > *j){
					++(*i);
				}
				break;
			}else{
				if(j == addedPath.end() - 1){
					++(*i);
					break;
				}
			}
		}
		iter.pathPtr = this->pos(iter.path()).pathPtr;
	}
	
	void resetChildren(Iterator childrenOf, size_t numberOfChildren){
		for(auto t : childrenOf.pathPtr){
//			TRACE(<< "t = " << t << std::endl)
			t->size_var -= (*childrenOf).size();
			t->size_var += numberOfChildren;
		}
		
		(*childrenOf).resetChildren(numberOfChildren);
	}
	
	void resetChildren(size_t numberOfChildren){
		this->children.clear();
		this->children.resize(this->children.size() + numberOfChildren);
		this->size_var = numberOfChildren;
	}
	
	void add(size_t before){
		this->children.insert(this->children.begin() + before, Tree());
		++this->size_var;
	}
	
	void add(Iterator before){
		for(auto t : before.pathPtr){
//			TRACE(<< "t = " << t << std::endl)
			++t->size_var;
		}
		
		before.parent().children.insert(before.parent().children.begin() + before.path().back(), Tree());
	}
	
	void remove(Iterator i){
		if(!i){
			return;
		}
		
		size_t numNodesToRemove = (*i).size() + 1;
		
		for(auto p : i.pathPtr){
			p->size_var -= numNodesToRemove;
		}
		
		size_t index = i.path().back();
		i.ascent();
		Tree* node;
		if(!i){
			node = this;
		}else{
			node = &(*i);
		}
		ASSERT(index < node->children.size())
		node->children.erase(node->children.begin() + index);
	}
	
	void removeAll(Iterator& from){
		size_t numChildrenToRemove = (*from).size();
		(*from).children.clear();
		(*from).size_var -= numChildrenToRemove;
		ASSERT((*from).size() == 0)
		for(auto t : from.pathPtr){
			ASSERT(t->size_var >= numChildrenToRemove)
			t->size_var -= numChildrenToRemove;
		}
	}
	
	void removeAll(){
		this->children.clear();
		this->size_var = 0;
	}
	
	decltype(size_var) size()const noexcept{
		return this->size_var;
	}
	
	size_t numChildren()const noexcept{
		return this->children.size();
	}
	
	Iterator begin(){
		if(this->children.size() == 0){
			return Iterator();
		}
		return Iterator(this, 0);
	}
	
	Iterator end(){
		return Iterator();
	}
	
	Iterator pos(const std::vector<size_t>& path){
		auto i = path.begin();
		if(i == path.end()){
			return this->end();
		}
		
		Iterator ret(this, *i);
		++i;
		
		for(; i != path.end(); ++i){
			auto oldDepth = ret.depth();
			ret.descent(*i);
			if(oldDepth == ret.depth()){
				break;
			}
		}
		
		return ret;
	}
	
private:

};



class TreeView :
		virtual public Widget,
		private ScrollContainer
{
	std::shared_ptr<List> list;
public:
	TreeView(const stob::Node* chain = nullptr);
	
	TreeView(const TreeView&) = delete;
	TreeView& operator=(const TreeView&) = delete;
	
	
	class ItemsProvider :
			public virtual utki::Shared,
			private List::ItemsProvider
	{
		friend class TreeView;
		friend std::shared_ptr<List::ItemsProvider> std::static_pointer_cast<List::ItemsProvider>(const std::shared_ptr<ItemsProvider>&)noexcept;
		
		void recycle(size_t index, std::shared_ptr<Widget> w)override;
		
		std::shared_ptr<Widget> getWidget(size_t index)override;
		
		size_t count() const noexcept override;
		
		mutable Tree visibleTree;
		
		//cached values for faster lookup by index
		mutable size_t iterIndex;
		mutable decltype(visibleTree)::Iterator iter;
		
		const decltype(iter)& iterForIndex(size_t index)const;
		
	protected:
		ItemsProvider(){
		}
	public:
		
		virtual std::shared_ptr<Widget> getWidget(const std::vector<size_t>& path, bool isCollapsed) = 0;
		
		virtual void recycle(const std::vector<size_t>& path, std::shared_ptr<Widget> w){}
		
		virtual size_t count(const std::vector<size_t>& path)const noexcept = 0;
		
		void uncollapse(const std::vector<size_t>& path);
		void collapse(const std::vector<size_t>& path);
		
		void notifyDataSetChanged();
		
		void notifyItemChanged(){
			this->List::ItemsProvider::notifyDataSetChanged();
		}
		
		/**
		 * @brief Notify that an item has been removed.
		 * @param path - index path of the removed item.
		 */
		void notifyItemRemoved(const std::vector<size_t>& path);
		
		/**
		 * @brief Notify that a new item has been added.
		 * @param path - index path to a newly added item. Essentially, it is a path
		 *               to an item before which a new item has been added.
		 */
		void notifyItemAdded(const std::vector<size_t>& path);
		
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
		return Vec2r(this->ScrollContainer::scrollFactor().x, this->list->scrollFactor());
	}
};



}
