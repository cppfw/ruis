#include "TreeView.hpp"
#include "../App.hpp"



using namespace morda;



TreeView::TreeView(const stob::Node* chain) :
		Widget(chain)
{
	this->list = utki::makeShared<VerticalList>();
	this->add(this->list);
	
	auto& lp = this->getLayoutParams(*this->list);
	
	lp.dim.y = Widget::LayoutParams::D_Max;
	lp.dim.x = Widget::LayoutParams::D_Min;
	
	this->list->dataSetChanged = [this](List&){
		if(this->viewChanged){
			this->viewChanged(*this);
		}
	};
}

void TreeView::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	this->list->setItemsProvider(
			std::static_pointer_cast<List::ItemsProvider>(provider)
		);
}

void TreeView::ItemsProvider::notifyDataSetChanged() {
	this->visibleTree.removeAll();
	this->iterIndex = 0;
	this->iter = this->visibleTree.begin();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count() const noexcept{
	if(this->visibleTree.size() == 0){
		auto size = this->count(std::vector<size_t>());
		this->visibleTree.resetChildren(size);
		this->iter = this->visibleTree.begin();
		this->iterIndex = 0;
	}
	return this->visibleTree.size();
}


std::shared_ptr<Widget> TreeView::ItemsProvider::getWidget(size_t index){
	auto i = this->iterForIndex(index);
	
	return this->getWidget(i.path(), (*i).numChildren() == 0);
}

void TreeView::ItemsProvider::recycle(size_t index, std::shared_ptr<Widget> w){
	auto i = this->iterForIndex(index);
	
	this->recycle(i.path(), std::move(w));
}

const decltype(TreeView::ItemsProvider::iter)& TreeView::ItemsProvider::iterForIndex(size_t index) const {
	ASSERT(this->iter.path().size() != 0)
	
	if(index != this->iterIndex){
		if(index > this->iterIndex){
			this->iter += index - this->iterIndex;
		}else{ ASSERT(index < this->iterIndex)
			this->iter -= this->iterIndex - index;
		}
		this->iterIndex = index;
	}
	
	return this->iter;
}

void TreeView::ItemsProvider::collapse(const std::vector<size_t>& path) {
	auto i = this->visibleTree.pos(path);
	ASSERT(i != this->visibleTree.end())
	
	if(this->iter > i){
		auto pnext = path;
		++pnext.back();
		
		if(this->iter.path() < pnext){
			while(this->iter != i){
				--this->iter;
				--this->iterIndex;
			}
		}else{
			this->iterIndex -= (*i).size();
		}
	}
	
	this->visibleTree.removeAll(i);
	
	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::uncollapse(const std::vector<size_t>& path) {
	auto s = this->count(path);
//	TRACE(<< "TreeView::ItemsProvider::uncollapse(): s = " << s << std::endl)
	if(s == 0){
		return;
	}
	
	auto i = this->visibleTree.pos(path);
	ASSERT(i != this->visibleTree.end())
	
	ASSERT((*i).numChildren() == 0)
	
	if(this->iter > i){
		this->iterIndex += s;
	}
	
	this->visibleTree.resetChildren(i, s);
	
	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::notifyItemAdded(const std::vector<size_t>& path) {
	auto i = this->visibleTree.pos(path);
	if(!i || i.path().back() > i.parent().numChildren()){
		return;
	}
	
	if(i.parent().numChildren() == 0){
		this->List::ItemsProvider::notifyDataSetChanged();
		return;
	}
	
	if(this->iter >= i){
		++this->iterIndex;
	}
	
	this->visibleTree.add(i);
	this->visibleTree.correctIteratorAfterAddition(this->iter, i.path());
	
	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::notifyItemRemoved(const std::vector<size_t>& path) {
	auto i = this->visibleTree.pos(path);
//	TRACE(<< " sss = " << i.path()[0] << " iter = " << this->iter.path()[0] << std::endl)
	
	if(this->iter >= i){
		auto pnext = path;
		++pnext.back();
		
		if(this->iter.path() < pnext){
			while(this->iter != i){
				ASSERT(this->iterIndex != 0)
				--this->iter;
				--this->iterIndex;
			}
		}else{
			this->iterIndex -= ((*i).size() + 1);
		}
	}
	
	this->visibleTree.remove(i);
	this->visibleTree.correctIteratorAfterDeletion(this->iter, i.path());
	
	this->List::ItemsProvider::notifyDataSetChanged();
}
