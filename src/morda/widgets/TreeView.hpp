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
		static void pathFromPlainIndex(size_t index, const std::vector<Item>& items, std::vector<size_t>& path);
	};
private:

public:
	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);
	
	
};



}
