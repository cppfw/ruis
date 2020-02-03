#include "TreeView.hpp"

#include "../../Morda.hpp"



using namespace morda;



TreeView::TreeView(const puu::forest& desc) :
		widget(desc),
		ScrollArea(puu::forest())
{
	this->list = std::make_shared<VList>(puu::forest());
	this->push_back(this->list);

	auto& lp = this->get_layout_params(*this->list);

	lp.dim.y = Widget::LayoutParams::max_c;
	lp.dim.x = Widget::LayoutParams::min_c;

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
	this->visible_tree.children.clear();
	this->visible_tree.value = 0;
	// this->visibleTree.clear();
	this->iter_index = 0;
	this->iter = utki::make_traversal(this->visible_tree.children).begin();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count() const noexcept{
	// if(this->visibleTree.size() == 0){
	// 	auto size = this->count(std::vector<size_t>());
	// 	this->visibleTree.resetChildren(size);
	// 	this->iter = this->visibleTree.begin();
	// 	this->iterIndex = 0;
	// }
	return this->visible_tree.value;
}


std::shared_ptr<Widget> TreeView::ItemsProvider::getWidget(size_t index){
	auto& i = this->iter_for(index);

//	TRACE(<< "i.path() = " << (*i).numChildren() << std::endl)

	return this->getWidget(i.index(), i->value == 0);
}

void TreeView::ItemsProvider::recycle(size_t index, std::shared_ptr<Widget> w){
	auto& i = this->iter_for(index);

	this->recycle(i.index(), std::move(w));
}

const decltype(TreeView::ItemsProvider::iter)& TreeView::ItemsProvider::iter_for(size_t index)const{
	auto traversal = utki::make_traversal(this->visible_tree.children);

	if(index != this->iter_index){
		if(index > this->iter_index){
			this->iter = std::next(this->iter, index - this->iter_index);
		}else{
			ASSERT(index < this->iterIndex)
			this->iter = std::prev(this->iter, this->iter_index - index);
		}
		this->iter_index = index;
	}

	return this->iter;
}

void TreeView::ItemsProvider::collapse(const std::vector<size_t>& index) {
	auto traversal = utki::make_traversal(this->visible_tree.children);
	ASSERT(traversal.is_valid(index))
	
	auto i = this->visibleTree.pos(index);
	ASSERT(i != this->visibleTree.end())

	if(this->iter > i){
		auto pnext = index;
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
