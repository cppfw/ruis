#include "TreeView.hpp"
#include "List.hpp"



using namespace morda;



TreeView::TreeView(const stob::Node* chain) :
		Widget(chain)
{
	this->Add(ting::New<VerticalList>());
}

void TreeView::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(this->provider){
		this->provider->treeView = nullptr;
	}
	this->provider = std::move(provider);
	if(this->provider){
		this->provider->treeView = this;
	}
	this->notifyDataSetChanged();
}
