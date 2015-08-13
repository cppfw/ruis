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
	this->numVisibleItems = 0;
	this->visibleItemsTree.clear();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count() const noexcept{
	if (this->numVisibleItems == 0) {
		this->numVisibleItems = this->count(std::vector<size_t>());
		this->visibleItemsTree.resize(this->numVisibleItems);
	}
	return this->numVisibleItems;
}


std::shared_ptr<Widget> TreeView::ItemsProvider::getWidget(size_t index) const {
	//TODO:
	return nullptr;
}

void TreeView::ItemsProvider::recycle(size_t index, std::shared_ptr<Widget> w) const {
	//TODO:
}
