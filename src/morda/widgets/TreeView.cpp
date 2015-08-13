#include "TreeView.hpp"



using namespace morda;



TreeView::TreeView(const stob::Node* chain) :
		Widget(chain)
{
	this->list = ting::New<VerticalList>();
	this->Add(this->list);
	
	auto& lp = this->getLayoutParams(*this->list);
	
	lp.dim.y = Widget::LayoutParams::D_Max;
	lp.dim.x = Widget::LayoutParams::D_Min;
}

void TreeView::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	this->list->setItemsProvider(
			std::shared_ptr<List::ItemsProvider>(
					static_cast<List::ItemsProvider*>(provider.get())
				)
		);
}

void TreeView::ItemsProvider::notifyDataSetChanged() {
	this->visibleItemsTree.reset();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count() const noexcept{
	if (this->visibleItemsTree.numUnderlyingVisible == 0) {
		ASSERT(this->visibleItemsTree.children.size() == 0)
		this->visibleItemsTree.init(this->count(ting::Buffer<const size_t>()));
	}
	return this->visibleItemsTree.numUnderlyingVisible;
}


std::shared_ptr<Widget> TreeView::ItemsProvider::getWidget(size_t index) const {
	//TODO:
	return nullptr;
}

void TreeView::ItemsProvider::recycle(size_t index, std::shared_ptr<Widget> w) const {
	std::vector<size_t> path;
	
	//TODO:
}


void TreeView::ItemsProvider::pathFromPlainIndex(size_t index, const std::vector<Item>& items, std::vector<size_t>& path) {
	//TODO:
//	size_t idx = 0;
//	
//	for(Item i : items){
//		if(index == 0){
//			path.push_back(idx);
//			return;
//		}
//		--index;
//		if(i.children.size() != 0){
//			
//		}
//		
//		
//		if(index < i.children.size()){
//			path.push_back(idx);
//			//TODO:
//		}else{
//			index -= i.children.size();
//			++idx;
//		}
//	}
}


