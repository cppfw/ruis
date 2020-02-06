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
	this->visible_tree.value.subtree_size = 0;
	this->iter_index = 0;
	this->iter = this->traversal().begin();
	this->List::ItemsProvider::notifyDataSetChanged();
}


size_t TreeView::ItemsProvider::count()const noexcept{
	if(this->visible_tree.value.subtree_size == 0){
		ASSERT(this->visible_tree.children.empty())
		auto size = this->count(std::vector<size_t>());
		if(size != 0){
			this->visible_tree.value.subtree_size = size;
			this->visible_tree.children.resize(size);
			this->iter = this->traversal().begin();
			this->iter_index = 0;
		}
	}
	return this->visible_tree.value.subtree_size;
}


std::shared_ptr<Widget> TreeView::ItemsProvider::getWidget(size_t index){
	auto& i = this->iter_for(index);

	return this->getWidget(i.index(), i->value.subtree_size == 0);
}

void TreeView::ItemsProvider::recycle(size_t index, std::shared_ptr<widget> w){
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
	auto num_to_remove = from->value.subtree_size;
	auto index = from.index();

	from->children.clear();
	from->value.subtree_size = 0;

	auto p = &this->visible_tree;
	for(auto t : index){
		p->value.subtree_size -= num_to_remove;
		p = &p->children[t];
	}
	ASSERT(p->children.empty())
	ASSERT(p->value.subtree_size == 0)
}

void TreeView::ItemsProvider::collapse(const std::vector<size_t>& index) {
	ASSERT(this->traversal().is_valid(index))

	auto i = this->traversal().make_iterator(index);

	if(this->iter > i){
		auto next_index = index;
		++next_index.back();

		if(this->iter.index() < next_index){
			while(this->iter != i){
				--this->iter;
				--this->iter_index;
			}
		}else{
			this->iter_index -= i->value.subtree_size;
		}
	}

	// iterator is invalidated after removing children from the tree node, so save its index to re-create it after
	auto ii = this->iter.index();

	this->remove_children(i);

	ASSERT(this->traversal().is_valid(ii))
	this->iter = this->traversal().make_iterator(ii);

	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::set_children(decltype(iter) i, size_t num_children){
	auto index = i.index();
	ASSERT(this->traversal().is_valid(index));

	auto old_num_children = i->value.subtree_size;

	auto p = &this->visible_tree;
	for(auto t : index){
		p->value.subtree_size -= old_num_children;
		p->value.subtree_size += num_children;
		p = &p->children[t];
	}

	ASSERT(p == i.operator->())

	i->children.clear();
	i->children.resize(num_children);
	i->value.subtree_size = num_children;
}

void TreeView::ItemsProvider::uncollapse(const std::vector<size_t>& index) {
	auto num_children = this->count(index);
//	TRACE(<< "TreeView::ItemsProvider::uncollapse(): s = " << s << std::endl)
	if(num_children == 0){
		return;
	}

	ASSERT(this->traversal().is_valid(index))
	auto i = this->traversal().make_iterator(index);

	ASSERT(i->value.subtree_size == 0)

	if(this->iter > i){
		this->iter_index += num_children;
	}

	auto ii = this->iter.index();

	this->set_children(i, num_children);

	this->iter = this->traversal().make_iterator(ii);

	this->List::ItemsProvider::notifyDataSetChanged();
}

void TreeView::ItemsProvider::notifyItemAdded(const std::vector<size_t>& index) {
	if(index.empty()){
		throw std::invalid_argument("passed in index is empty");
	}

	// find parent tree node list to which the new node was added
	auto parent_index = utki::make_span(index.data(), index.size() - 1);
	decltype(this->visible_tree.children)* parent_list;
	if(parent_index.empty()){ // if added to root node
		parent_list = &this->visible_tree.children;
	}else{
		ASSERT(this->traversal().is_valid(parent_index))
		auto parent_iter = this->traversal().make_iterator(parent_index);
		parent_list = &parent_iter->children;
	}

	if(parent_list->empty()){
		// item was added to a collapsed subtree
		this->List::ItemsProvider::notifyDataSetChanged();
		return;
	}

	auto old_iter_index = this->iter.index();
	if(old_iter_index >= index){
		++this->iter_index;
	}

	// TRACE(<< "parent_list->size() = " << parent_list->size() << std::endl)

	parent_list->insert(std::next(parent_list->begin(), index.back()), node());

	// TRACE(<< "parent_list->size() = " << parent_list->size() << std::endl)

	auto p = &this->visible_tree;
	for(auto k : index){
		++p->value.subtree_size;
		p = &p->children[k];
	}

	ASSERT(p->value.subtree_size == 0)

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

void TreeView::ItemsProvider::notifyItemRemoved(const std::vector<size_t>& index){
	if(index.empty()){
		throw std::invalid_argument("passed in index is empty");
	}

	if(!this->traversal().is_valid(index)){
		// the removed item was probably in collapsed part of the tree
		this->List::ItemsProvider::notifyDataSetChanged();
		return;
	}

	auto ri = this->traversal().make_iterator(index);

	auto cur_iter_index = this->iter.index();
	if(cur_iter_index >= index){
		auto next_index = index;
		++next_index.back();

		if(cur_iter_index < next_index){
			while(this->iter != ri){
				ASSERT(this->iter_index != 0)
				--this->iter;
				--this->iter_index;
			}
		}else{
			this->iter_index -= (ri->value.subtree_size + 1);
		}
	}

	cur_iter_index = this->iter.index();

	this->traversal().erase(ri);

	// correct current iterator after deletion
	{
		auto i = cur_iter_index.begin();
		auto j = index.begin();
		for(; i != cur_iter_index.end() && j != index.end(); ++i, ++j){
			if(*i != *j){
				if(j != index.end() - 1){
					break; // items are in different branches, no correction is needed
				}

				if(*i > *j){
					--(*i);
				}
				break;
			}else{
				if(j == index.end() - 1){
					cur_iter_index = index;
					break;
				}
			}
		}
	}
	ASSERT(this->traversal().is_valid(cur_iter_index))
	ASSERT(cur_iter_index.size() != 0)
	while(cur_iter_index.size() != 1){
		auto parent_iter_span = utki::make_span(cur_iter_index.data(), cur_iter_index.size() - 1);
		ASSERT(this->traversal().is_valid(parent_iter_span));
		auto parent_iter = this->traversal().make_iterator(parent_iter_span);
		if(parent_iter->children.size() != cur_iter_index.back()){
			break;
		}
		cur_iter_index.pop_back();
		ASSERT(!cur_iter_index.empty())
		++cur_iter_index.back();
	}
	this->iter = this->traversal().make_iterator(cur_iter_index);

	this->List::ItemsProvider::notifyDataSetChanged();
}
