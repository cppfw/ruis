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

void TreeView::ItemsProvider::notifyDataSetChanged(){
	this->visible_tree.children.clear();
	this->visible_tree.value = 0;
	this->iter_index = 0;
	this->iter = this->traversal().begin();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count()const noexcept{
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
	if(index != this->iter_index){
		if(index > this->iter_index){
			this->iter = std::next(this->iter, index - this->iter_index);
		}else{
			ASSERT(index < this->iter_index)
			this->iter = std::prev(this->iter, this->iter_index - index);
		}
		this->iter_index = index;
	}

	return this->iter;
}

void TreeView::ItemsProvider::remove_children(decltype(iter) from){
	auto num_to_remove = from->value;
	from->children.clear();
	from->value = 0;

	auto index = from.index();

	auto p = &this->visible_tree;
	for(auto t : index){
		p->value -= num_to_remove;
		p = &p->children[t];
	}
	ASSERT(p->children[index.back()] == 0)
}

void TreeView::ItemsProvider::collapse(const std::vector<size_t>& index) {
	ASSERT(this->traversal().is_valid(index))

	auto i = this->traversal().make_iterator(index);

	if(this->iter > i){
		auto pnext = index;
		++pnext.back();

		if(this->iter.index() < pnext){
			while(this->iter != i){
				--this->iter;
				--this->iter_index;
			}
		}else{
			this->iter_index -= i->value;
		}
	}

	this->remove_children(i);

	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::set_children(decltype(iter) i, size_t num_children){
	auto index = i.index();
	ASSERT(this->traversal().is_valid(index));

	auto old_num_children = i->value;

	auto p = &this->visible_tree;
	for(auto t : index){
		p->value -= old_num_children;
		p->value += num_children;
		p = &p->children[t];
	}

	i->children.clear();
	i->children.resize(num_children);
	i->value = num_children;
	for(auto& k : i->children){
		k = 0;
	}
}

void TreeView::ItemsProvider::uncollapse(const std::vector<size_t>& index) {
	auto num_children = this->count(index);
//	TRACE(<< "TreeView::ItemsProvider::uncollapse(): s = " << s << std::endl)
	if(num_children == 0){
		return;
	}

	ASSERT(this->traversal().is_valid(index))
	auto i = this->traversal().make_iterator(index);

	ASSERT(i->value == 0)

	if(this->iter > i){
		this->iter_index += num_children;
	}

	this->set_children(i, num_children);

	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::notifyItemAdded(const std::vector<size_t>& index) {
	if(index.empty()){
		throw std::invalid_argument("passed in index is empty");
	}
	auto parent_index = utki::make_span(index.data(), index.size() - 1);
	if(!this->traversal().is_valid(parent_index)){
		return;
	}
	auto parent_iter = this->traversal().make_iterator(parent_index);

	auto old_iter_index = this->iter.index();
	if(old_iter_index >= index){
		++this->iter_index;
	}

	parent_iter->children.insert(std::next(parent_iter->children.begin(), index.back()), 0);

	auto p = &this->visible_tree;
	for(auto k : index){
		++p->value;
		p = &p->children[k];
	}

	// correct current iterator after insertion

	auto i = old_iter_index.begin();
	auto j = index.begin();
	for(; i != old_iter_index.end() && j != index.end(); ++i, ++j){
		if(*i != *j){
			if(j != index.end() - 1){
				break; // items are in different branches, no correction needed
			}

			if(*i > *j){
				++(*i);
			}
			break;
		}else{
			if(j == index.end() - 1){
				++(*i);
				break;
			}
		}
	}
	this->iter = this->traversal().make_iterator(old_iter_index);

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
