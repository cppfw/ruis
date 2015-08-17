/* The MIT License:

Copyright (c) 2015 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Widget.hpp"
#include "List.hpp"

#include "containers/ScrollContainer.hpp"

#include <ting/Buffer.hpp>


namespace morda{


class Tree{
public:
	std::vector<Tree> children;
	
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
		
		const decltype(pathIdx)& path()const noexcept{
			return this->pathIdx;
		}
		
		bool operator==(const Iterator& i){
			return this->pathIdx == i.pathIdx;
		}
		
		bool operator!=(const Iterator& i){
			return !this->operator==(i);
		}
		
		Tree& operator*(){
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
		
		Iterator& operator--(){
			if(this->pathIdx.size() == 0){
				return *this;
			}
			
			{
				auto& idx = this->pathIdx.back();

				if(idx == 0){
					this->pathPtr.pop_back();
					this->pathIdx.pop_back();
					return *this;
				}
			}
			
			for(;;){
				auto& list = this->pathPtr.back()->children;
				auto& idx = this->pathIdx.back();
				
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
		
		Iterator& descentTo(size_t index){
			if(index >= this->operator*().children.size()){
				return *this;
			}
			this->pathIdx.push_back(index);
			this->pathPtr.push_back(&this->operator *().children[index]);
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
	};
	
	Iterator begin(){
		if(this->children.size() == 0){
			return Iterator();
		}
		return Iterator(this, 0);
	}
	
	Iterator end(){
		return Iterator();
	}
	
	Iterator pos(const std::vector<size_t> path){
		Iterator ret;
		
		for(auto i = path.begin(); i != path.end(); ++i){
			ret.descentTo(*i);
			if(ret == this->end()){
				return this->end();
			}
		}
		
		return std::move(ret);
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
			public virtual ting::Shared,
			private List::ItemsProvider
	{
		friend class TreeView;
		
		void recycle(size_t index, std::shared_ptr<Widget> w) const override;
		
		std::shared_ptr<Widget> getWidget(size_t index)const override;
		
		size_t count() const noexcept override;
		
		struct Item{
			size_t numUnderlyingVisible = 0;
			std::vector<Item> children;
			
			void reset(){
				this->numUnderlyingVisible = 0;
				this->children.clear();
			}
			
			void init(size_t numVisible){
				this->children.resize(numVisible);
				this->numUnderlyingVisible = numVisible;
			}
		};
		
		mutable Item visibleItemsTree;
		
		//cached values for faster lookup by index
		size_t curIndex = 0;
		std::vector<size_t> curPath;
		
	protected:
		ItemsProvider(){
		}
	public:
		
		virtual std::shared_ptr<Widget> getWidget(ting::Buffer<const size_t> path, bool isCollapsed)const = 0;
		
		virtual void recycle(ting::Buffer<const size_t> path, std::shared_ptr<Widget> w)const{}
		
		virtual size_t count(ting::Buffer<const size_t> path)const noexcept = 0;
		
		void notifyDataSetChanged();
		
		//TODO:
		
	private:
		
	};
private:

public:
	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);
	
	
};



}
