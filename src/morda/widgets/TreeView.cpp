#include "TreeView.hpp"
#include "../App.hpp"



using namespace morda;



TreeView::TreeView(const stob::Node* chain) :
		Widget(chain)
{
	this->list = ting::New<VerticalList>();
	this->Add(this->list);
	
	auto& lp = this->getLayoutParams(*this->list);
	
	lp.dim.y = Widget::LayoutParams::D_Max;
	lp.dim.x = Widget::LayoutParams::D_Min;
	
	
	
	
	
	Tree t;
	
	t.add(4);
	
	t.add(t.pos({0}), 1);
	
	t.add(t.pos({2}), 3);
	
	t.add(t.pos({2, 1}), 2);
	
	
	for(auto i = t.begin(); i != t.end(); ++i){
		for(auto s : i.path()){
			TRACE_ALWAYS(<< s << " ")
		}
		TRACE_ALWAYS(<< std::endl)
	}
	
	TRACE_ALWAYS(<< "total size = " << t.size() << std::endl)
	
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
		this->visibleTree.add(size);
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

const decltype(TreeView::ItemsProvider::iter) TreeView::ItemsProvider::iterForIndex(size_t index) const {
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
	//TODO:
}

void TreeView::ItemsProvider::uncollapse(const std::vector<size_t>& path) {
	auto s = this->count(path);
	TRACE(<< "TreeView::ItemsProvider::uncollapse(): s = " << s << std::endl)
	if(s == 0){
		return;
	}
	
	auto i = this->visibleTree.pos(path);
	ASSERT(i != this->visibleTree.end())
	
	ASSERT((*i).numChildren() == 0)
	
	//TODO: check if iterIndex needs to be corrected
	
	this->visibleTree.add(i, s);
	
	this->List::ItemsProvider::notifyDataSetChanged();
}
