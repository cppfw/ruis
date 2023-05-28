/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "tree_view.hpp"

#include "../../context.hpp"
#include "../../layouts/linear_layout.hpp"
#include "../label/image.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace morda;

tree_view::tree_view(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	scroll_area(this->context, treeml::forest()),
	item_list(utki::make_shared<morda::list>(this->context, treeml::forest()))
{
	this->push_back(this->item_list);

	auto& lp = this->item_list.get().get_layout_params();

	lp.dims.y() = layout_params::max;
	lp.dims.x() = layout_params::min;

	this->item_list.get().data_set_change_handler = [this](list_widget&) {
		this->notify_view_change();
	};

	this->item_list.get().scroll_change_handler = [this](list_widget& lw) {
		this->notify_view_change();
	};

	this->scroll_area::scroll_change_handler = [this](scroll_area& sa) {
		this->notify_view_change();
	};
}

void tree_view::notify_view_change()
{
	if (this->scroll_change_handler) {
		this->scroll_change_handler(*this);
	}
}

void tree_view::set_provider(std::shared_ptr<provider> item_provider)
{
	item_provider->notify_data_set_changed();
	this->item_list.get().set_provider(
		// use aliasing shared_ptr constructor becasue list_widget::provider
		// is a private base of tree_view::provider, so not possible
		// to use std::static_pointer_cast() because it does not see the
		// private base class
		std::shared_ptr<list_widget::provider>(item_provider, item_provider.get())
	);
}

void tree_view::provider::notify_data_set_changed()
{
	auto size = this->count(nullptr);
	this->visible_tree.children.clear();
	this->visible_tree.children.resize(size);
	this->visible_tree.value.subtree_size = size;
	this->iter = this->traversal().begin();
	this->iter_index = 0;
	this->list_widget::provider::notify_data_set_change();
}

size_t tree_view::provider::count() const noexcept
{
	return this->visible_tree.value.subtree_size;
}

namespace {
const treeml::forest plus_minus_layout = treeml::read(R"qwertyuiop(
		@pile{
			@image{
				id{plusminus}
			}
			@mouse_proxy{
				lp{
					dx{fill} dy{fill}
				}
				id{plusminus_mouseproxy}
			}
		}
	)qwertyuiop");

const treeml::forest vert_line_layout = treeml::read(R"qwertyuiop(
		@pile{
			lp{dx{${morda_tree_view_indent}} dy{fill}}
			@color{
				lp{dx{1pt}dy{fill}}
				color{${morda_color_highlight}}
			}
		}
	)qwertyuiop");

const treeml::forest line_end_layout = treeml::read(R"qwertyuiop(
		@pile{
			lp{dx{${morda_tree_view_indent}} dy{max}}
			@column{
				lp{dx{max}dy{max}}
				@color{
					lp{dx{1pt}dy{0}weight{1}}
					color{${morda_color_highlight}}
				}
				@widget{lp{dx{max}dy{0}weight{1}}}
			}
			@row{
				lp{dx{max}dy{max}}
				@widget{lp{dx{0}dy{max}weight{1}}}
				@color{
					lp{dx{0}dy{1pt}weight{1}}
					color{${morda_color_highlight}}
				}
			}
		}
	)qwertyuiop");

const treeml::forest line_middle_layout = treeml::read(R"qwertyuiop(
		@pile{
			lp{dx{${morda_tree_view_indent}} dy{max}}
			@color{
				lp{dx{1pt}dy{max}}
				color{${morda_color_highlight}}
			}
			@row{
				lp{dx{max}dy{max}}
				@widget{lp{dx{0}dy{max}weight{1}}}
				@color{
					lp{dx{0}dy{1pt}weight{1}}
					color{${morda_color_highlight}}
				}
			}
		}
	)qwertyuiop");

const treeml::forest empty_layout = treeml::read(R"qwertyuiop(
		@widget{lp{dx{${morda_tree_view_indent}}dy{0}}}
	)qwertyuiop");
} // namespace

utki::shared_ref<widget> tree_view::provider::get_widget(size_t index)
{
	auto& i = this->iter_for(index);

	auto path = i.index();
	bool is_collapsed = i->value.subtree_size == 0;

	auto list = &this->visible_tree.children;

	std::vector<bool> is_last_item_in_parent;

	decltype(this->visible_tree)* n = nullptr;

	for (const auto& i : path) {
		is_last_item_in_parent.push_back(i + 1 == list->size());
		n = &(*list)[i];
		list = &n->children;
	}

	ASSERT(this->get_list(), [&](auto& o) {
		o << "provider is not set to a list_widget";
	})

	auto ret = utki::make_shared<morda::container>(this->get_list()->context, treeml::forest(), row_layout::instance);

	ASSERT(is_last_item_in_parent.size() == path.size())

	for (unsigned i = 0; i != path.size() - 1; ++i) {
		ret.get().push_back_inflate(is_last_item_in_parent[i] ? empty_layout : vert_line_layout);
	}

	{
		auto widget = this->get_list()->context.get().inflater.inflate_as<morda::container>(
			is_last_item_in_parent.back() ? line_end_layout : line_middle_layout
		);

		if (this->count(utki::make_span(path)) != 0) {
			auto w = this->get_list()->context.get().inflater.inflate(plus_minus_layout);

			auto plusminus = w.get().try_get_widget_as<morda::image>("plusminus");
			ASSERT(plusminus)
			plusminus->set_image(
				(is_collapsed
					 ? this->get_list()->context.get().loader.load<morda::res::image>("morda_img_treeview_plus")
					 : this->get_list()->context.get().loader.load<morda::res::image>("morda_img_treeview_minus"))
					.to_shared_ptr()
			);

			auto plusminus_mouse_proxy = w.get().try_get_widget_as<morda::mouse_proxy>("plusminus_mouseproxy");
			ASSERT(plusminus_mouse_proxy)
			plusminus_mouse_proxy->mouse_button_handler =
				[this, path, is_collapsed](morda::mouse_proxy&, const morda::mouse_button_event& e) -> bool {
				if (e.button != morda::mouse_button::left) {
					return false;
				}
				if (!e.is_down) {
					return false;
				}

				if (is_collapsed) {
					this->uncollapse(utki::make_span(path));
				} else {
					this->collapse(utki::make_span(path));
				}

				utki::log([](auto& o) {
					o << "plusminus clicked:";
				});
				for (const auto& p : path) {
					utki::log([&](auto& o) {
						o << " " << p;
					});
				}
				utki::log([](auto& o) {
					o << std::endl;
				});

				return true;
			};
			widget.get().push_back(w);
		}
		ret.get().push_back(widget);
	}

	ret.get().push_back(this->get_widget(utki::make_span(path), is_collapsed));

	return ret;
}

void tree_view::provider::recycle(size_t index, const utki::shared_ref<widget>& w)
{
	auto& i = this->iter_for(index);

	auto path = i.index();
	this->recycle(utki::make_span(path), w);
}

const decltype(tree_view::provider::iter)& tree_view::provider::iter_for(size_t index) const
{
	if (index != this->iter_index) {
		if (index > this->iter_index) {
			this->iter = utki::next(this->iter, index - this->iter_index);
		} else {
			ASSERT(index < this->iter_index)
			this->iter = std::prev(this->iter, this->iter_index - index);
		}
		this->iter_index = index;
	}

	return this->iter;
}

void tree_view::provider::remove_children(decltype(iter) from)
{
	auto num_to_remove = from->value.subtree_size;
	auto index = from.index();

	from->children.clear();
	from->value.subtree_size = 0;

	auto p = &this->visible_tree;
	for (auto t : index) {
		p->value.subtree_size -= num_to_remove;
		p = &p->children[t];
	}
	ASSERT(p->children.empty())
	ASSERT(p->value.subtree_size == 0)
}

void tree_view::provider::collapse(utki::span<const size_t> index)
{
	ASSERT(this->traversal().is_valid(index))

	auto i = this->traversal().make_iterator(index);

	if (this->iter > i) {
		auto next_index = utki::make_vector(index);
		++next_index.back();

		if (this->iter.index() < next_index) {
			while (this->iter != i) {
				--this->iter;
				--this->iter_index;
			}
		} else {
			this->iter_index -= i->value.subtree_size;
		}
	}

	// iterator is invalidated after removing children from the tree node, so save its index to re-create it after
	auto ii = this->iter.index();

	this->remove_children(i);

	ASSERT(this->traversal().is_valid(ii))
	this->iter = this->traversal().make_iterator(ii);

	this->list_widget::provider::notify_data_set_change();
}

void tree_view::provider::set_children(decltype(iter) i, size_t num_children)
{
	auto index = i.index();
	ASSERT(this->traversal().is_valid(index));

	auto old_subtree_size = i->value.subtree_size;

	auto p = &this->visible_tree;
	for (auto t : index) {
		p->value.subtree_size -= old_subtree_size;
		p->value.subtree_size += num_children;
		p = &p->children[t];
	}

	ASSERT(p == i.operator->())

	i->children.clear();
	i->children.resize(num_children);
	i->value.subtree_size = num_children;
}

void tree_view::provider::uncollapse(utki::span<const size_t> index)
{
	auto num_children = this->count(index);
	//	TRACE(<< "tree_view::provider::uncollapse(): s = " << s << std::endl)
	if (num_children == 0) {
		return;
	}

	ASSERT(this->traversal().is_valid(index))
	auto i = this->traversal().make_iterator(index);

	ASSERT(i->value.subtree_size == 0)

	if (this->iter > i) {
		this->iter_index += num_children;
	}

	auto ii = this->iter.index();

	this->set_children(i, num_children);

	this->iter = this->traversal().make_iterator(ii);

	this->list_widget::provider::notify_data_set_change();
}

void tree_view::provider::notify_item_added(utki::span<const size_t> index)
{
	if (index.empty()) {
		throw std::invalid_argument("passed in index is empty");
	}

	// find parent tree node list to which the new node was added
	auto parent_index = utki::make_span(index.data(), index.size() - 1);
	decltype(this->visible_tree.children)* parent_list;
	if (parent_index.empty()) { // if added to root node
		parent_list = &this->visible_tree.children;
	} else {
		ASSERT(this->traversal().is_valid(parent_index))
		auto parent_iter = this->traversal().make_iterator(parent_index);
		parent_list = &parent_iter->children;
	}

	if (parent_list->empty()) {
		// item was added to a collapsed subtree
		this->list_widget::provider::notify_data_set_change();
		return;
	}

	auto old_iter_index = this->iter.index();
	if (utki::make_span(old_iter_index) >= index) {
		++this->iter_index;
	}

	// TRACE(<< "parent_list->size() = " << parent_list->size() << std::endl)

	parent_list->insert(utki::next(parent_list->begin(), index.back()), node());

	// TRACE(<< "parent_list->size() = " << parent_list->size() << std::endl)

	auto p = &this->visible_tree;
	for (auto k : index) {
		++p->value.subtree_size;
		p = &p->children[k];
	}

	ASSERT(p->value.subtree_size == 0)

	// correct current iterator after insertion

	auto i = old_iter_index.begin();
	auto j = index.begin();
	for (; i != old_iter_index.end() && j != index.end(); ++i, ++j) {
		if (*i != *j) {
			if (j != index.end() - 1) {
				break; // items are in different branches, no correction needed
			}

			if (*i > *j) {
				++(*i);
			}
			break;
		} else {
			if (j == index.end() - 1) {
				++(*i);
				break;
			}
		}
	}
	this->iter = this->traversal().make_iterator(old_iter_index);

	this->list_widget::provider::notify_data_set_change();
}

void tree_view::provider::notify_item_removed(utki::span<const size_t> index)
{
	if (index.empty()) {
		throw std::invalid_argument("passed in index is empty");
	}

	if (!this->traversal().is_valid(index)) {
		// the removed item was probably in collapsed part of the tree
		this->list_widget::provider::notify_data_set_change();
		return;
	}

	auto ri = this->traversal().make_iterator(index);

	auto cur_iter_index = this->iter.index();
	if (utki::make_span(cur_iter_index) >= index) {
		auto next_index = utki::make_vector(index);
		++next_index.back();

		if (cur_iter_index < next_index) {
			while (this->iter != ri) {
				ASSERT(this->iter_index != 0)
				--this->iter;
				--this->iter_index;
			}
		} else {
			this->iter_index -= (ri->value.subtree_size + 1);
		}
	}

	cur_iter_index = this->iter.index();

	{
		auto removed_subtree_size = ri->value.subtree_size + 1;
		auto* p = &this->visible_tree;
		for (auto k : index) {
			p->value.subtree_size -= removed_subtree_size;
			p = &p->children[k];
		}
	}

	this->traversal().erase(ri);

	// correct current iterator after deletion
	{
		auto i = cur_iter_index.begin();
		auto j = index.begin();
		for (; i != cur_iter_index.end() && j != index.end(); ++i, ++j) {
			if (*i != *j) {
				if (j != index.end() - 1) {
					break; // items are in different branches, no correction is needed
				}

				if (*i > *j) {
					--(*i);
				}
				break;
			} else {
				if (j == index.end() - 1) {
					cur_iter_index = utki::make_vector(index);
					break;
				}
			}
		}
	}

	ASSERT(this->traversal().is_valid(cur_iter_index))
	ASSERT(cur_iter_index.size() != 0)
	while (cur_iter_index.size() != 1) {
		auto parent_iter_span = utki::make_span(cur_iter_index.data(), cur_iter_index.size() - 1);
		ASSERT(this->traversal().is_valid(parent_iter_span));
		auto parent_iter = this->traversal().make_iterator(parent_iter_span);
		if (parent_iter->children.size() != cur_iter_index.back()) {
			break;
		}
		cur_iter_index.pop_back();
		ASSERT(!cur_iter_index.empty())
		++cur_iter_index.back();
	}
	this->iter = this->traversal().make_iterator(cur_iter_index);

	this->list_widget::provider::notify_data_set_change();
}
