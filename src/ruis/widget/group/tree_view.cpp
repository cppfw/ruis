/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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
#include "../../default_style.hpp"
#include "../../layout/linear_layout.hpp"
#include "../label/gap.hpp"
#include "../label/image.hpp"
#include "../label/rectangle.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace std::string_literals;

using namespace ruis;

tree_view::tree_view( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	list(this->context,
		{
			.oriented_params{
				.vertical = true
			},
			.list_providable_params{
				.provider = [&](){
					class provider : public list_provider{
					public:
						provider(utki::shared_ref<ruis::context> context) :
							list_provider(std::move(context))
						{}

						size_t count() const noexcept override
						{
							return 0;
						}

						utki::shared_ref<ruis::widget> get_widget(size_t index) override
						{
							return make::gap(this->context, {});
						}
					};
					return utki::make_shared<provider>(context);
				}()
			}
		}
	)
{
	this->list::model_change_handler = [this](list&) {
		this->notify_view_change();
	};

	this->list::scroll_change_handler = [this](list&) {
		this->notify_view_change();
	};

	this->set_provider(std::move(params.tree_view_params.provider));
}

void tree_view::notify_view_change()
{
	if (this->scroll_change_handler) {
		this->scroll_change_handler(*this);
	}
}

void tree_view::set_provider(utki::shared_ref<provider> item_provider)
{
	// TODO: why do we need to do the notification here?
	item_provider.get().notify_model_change();

	auto p = item_provider.to_shared_ptr();

	this->list::set_provider(
		// use aliasing shared_ptr constructor becasue list::provider
		// is a private base of tree_view::provider, so not possible
		// to use std::static_pointer_cast() because it does not see the
		// private base class
		utki::shared_ref<list_provider>(std::shared_ptr<list_provider>(
			p, //
			p.get()
		))
	);
}

void tree_view::provider::notify_model_change()
{
	auto size = this->count({});
	this->visible_tree.children.clear();
	this->visible_tree.children.resize(size);
	this->visible_tree.value.subtree_size = size;
	this->iter = this->traversal().begin();
	this->iter_index = 0;
	this->list_provider::notify_model_change();
}

void tree_view::provider::notify_item_change()
{
	this->list_provider::notify_model_change();
}

size_t tree_view::provider::count() const noexcept
{
	return this->visible_tree.value.subtree_size;
}

namespace {
utki::shared_ref<ruis::container> make_line_end_indent(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return ruis::make::pile(c,
		{
			.layout_params{
				.dims{
					c.get().style().get_dim_indent_tree_view_item(),
					ruis::dim::max
				}
			}
    	},
		{
			ruis::make::column(c,
			 	{
					.layout_params{
						.dims{ruis::dim::max, ruis::dim::max}
					}
				},
			 	{
					ruis::make::rectangle(c,
				  		{
							.layout_params{
								.dims{ruis::length::make_pp(1), ruis::dim::fill},
								.weight = 1
							},
				   			.color_params{
								.color = c.get().style().get_color_highlight()
							}
						}
			  		),
			  		ruis::make::gap(c,
						{
							.layout_params{
								.dims{ruis::dim::max, ruis::dim::fill},
								.weight = 1
							}
						}
					)
				}
		 	),
		 	ruis::make::row(c,
			 	{
					.layout_params{
						.dims{ruis::dim::max, ruis::dim::max}
					}
				},
			 	{
					ruis::make::gap(c,
						{
							.layout_params{
								.dims{ruis::dim::fill, ruis::dim::max},
								.weight = 1
							}
						}
					),
			  		ruis::make::rectangle(c,
				  		{
							.layout_params{
								.dims{ruis::dim::fill, ruis::length::make_pp(1)},
								.weight = 1
							},
				   			.color_params{
								.color = c.get().style().get_color_highlight()
							}
						}
			  		)
				}
		 	)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::container> make_line_middle_indent(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return ruis::make::pile(c,
		{
			.layout_params{
				.dims{
					c.get().style().get_dim_indent_tree_view_item(),
					ruis::dim::max
				}
			}
		},
		{
			ruis::make::rectangle(c,
				{
					.layout_params{
						.dims{ruis::length::make_pp(1), ruis::dim::max}
					},
					.color_params{
						.color = c.get().style().get_color_highlight()
					}
				}
			),
			ruis::make::row(c,
				{
					.layout_params{
						.dims{ruis::dim::max, ruis::dim::max}
					}
				},
				{
					ruis::make::gap(c,
						{
							.layout_params{
								.dims{ruis::dim::fill, ruis::dim::max},
								.weight = 1
							}
						}
					),
					ruis::make::rectangle(c,
						{
							.layout_params{
								.dims{ruis::dim::fill, ruis::length::make_pp(1)},
								.weight = 1
							},
							.color_params{
								.color = c.get().style().get_color_highlight()
							}
						}
					)
				}
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_plus_minus_widget(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return ruis::make::pile(c,
		{},
		{
			ruis::make::image(c,
				{
					.widget_params{
						.id = "plusminus"s
					}
				}
			),
			ruis::make::mouse_proxy(c,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.id = "plusminus_mouseproxy"s
					}
				}
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_vertical_line_indent(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return ruis::make::pile(c,
		{
			.layout_params{
				.dims{
					c.get().style().get_dim_indent_tree_view_item(),
					ruis::dim::fill
				}
			}
		},
		{
			ruis::make::rectangle(c,
				{
					.layout_params{
						.dims{ruis::length::make_pp(1), ruis::dim::fill}
					},
					.color_params{
						.color = c.get().style().get_color_highlight()
					}
				}
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_empty_space_indent(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return ruis::make::gap(c,
		{
			.layout_params{
				.dims{
					c.get().style().get_dim_indent_tree_view_item(),
					ruis::length::make_px(0)
				}
			}
		}
	);
	// clang-format on
}
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

	widget_list prefix_widgets;

	ASSERT(is_last_item_in_parent.size() == path.size())

	for (unsigned i = 0; i != path.size() - 1; ++i) {
		if (is_last_item_in_parent[i]) {
			prefix_widgets.push_back(make_empty_space_indent(this->context));
		} else {
			prefix_widgets.push_back(make_vertical_line_indent(this->context));
		}
	}

	{
		auto widget = [&]() {
			if (is_last_item_in_parent.back()) {
				return make_line_end_indent(this->context);
			} else {
				return make_line_middle_indent(this->context);
			}
		}();

		if (this->count(utki::make_span(path)) != 0) {
			auto w = make_plus_minus_widget(this->context);

			auto& plusminus = w.get().get_widget_as<ruis::image>("plusminus");
			plusminus.set_image((is_collapsed
									 ? this->context.get().loader().load<ruis::res::image>("ruis_img_treeview_plus")
									 : this->context.get().loader().load<ruis::res::image>("ruis_img_treeview_minus"))
									.to_shared_ptr());

			auto plusminus_mouse_proxy = w.get().try_get_widget_as<ruis::mouse_proxy>("plusminus_mouseproxy");
			ASSERT(plusminus_mouse_proxy)
			plusminus_mouse_proxy->mouse_button_handler =
				[this, path, is_collapsed](ruis::mouse_proxy&, const ruis::mouse_button_event& e) -> bool {
				if (e.button != ruis::mouse_button::left) {
					return false;
				}
				if (!e.is_down) {
					return false;
				}

				if (is_collapsed) {
					this->expand(utki::make_span(path));
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
		prefix_widgets.emplace_back(std::move(widget));
	}

	return this->get_widget(
		utki::make_span(path), //
		std::move(prefix_widgets)
	);
}

utki::shared_ref<widget> tree_view::provider::get_widget(
	utki::span<const size_t> index, //
	widget_list prefix_widgets
)
{
	prefix_widgets.emplace_back(this->get_widget(index));

	return make::row(
		this->context, //
		{},
		std::move(prefix_widgets)
	);
}

void tree_view::provider::recycle(size_t index, utki::shared_ref<widget> w)
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
			this->iter = utki::prev(this->iter, this->iter_index - index);
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

	this->list_provider::notify_model_change();
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

void tree_view::provider::expand(utki::span<const size_t> index)
{
	auto num_children = this->count(index);
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

	this->list_provider::notify_model_change();
}

void tree_view::provider::notify_item_add(utki::span<const size_t> index)
{
	if (index.empty()) {
		throw std::invalid_argument("passed in index is empty");
	}

	// find parent tree node list to which the new node was added
	auto parent_index = utki::make_span(index.data(), index.size() - 1);
	decltype(this->visible_tree.children)* parent_list = [&]() {
		if (parent_index.empty()) { // if added to root node
			return &this->visible_tree.children;
		} else {
			ASSERT(this->traversal().is_valid(parent_index))
			auto parent_iter = this->traversal().make_iterator(parent_index);
			return &parent_iter->children;
		}
	}();

	if (parent_list->empty()) {
		// item was added to a collapsed subtree
		this->list_provider::notify_model_change();
		return;
	}

	auto old_iter_index = this->iter.index();
	if (utki::deep_greater_or_equals(utki::make_span(old_iter_index), index)) {
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
	for (; i != old_iter_index.end() && j != index.end(); i = std::next(i), j = std::next(j)) {
		if (*i != *j) {
			if (j != std::prev(index.end())) {
				break; // items are in different branches, no correction needed
			}

			if (*i > *j) {
				++(*i);
			}
			break;
		} else {
			if (j == std::prev(index.end())) {
				++(*i);
				break;
			}
		}
	}
	this->iter = this->traversal().make_iterator(old_iter_index);

	this->list_provider::notify_model_change();
}

void tree_view::provider::notify_item_remove(utki::span<const size_t> index)
{
	if (index.empty()) {
		throw std::invalid_argument("passed in index is empty");
	}

	if (!this->traversal().is_valid(index)) {
		// the removed item was probably in collapsed part of the tree
		this->list_provider::notify_model_change();
		return;
	}

	auto ri = this->traversal().make_iterator(index);

	auto cur_iter_index = this->iter.index();
	if (utki::deep_greater_or_equals(utki::make_span(cur_iter_index), index)) {
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
		for (; i != cur_iter_index.end() && j != index.end(); i = std::next(i), j = std::next(j)) {
			if (*i != *j) {
				if (j != std::prev(index.end())) {
					break; // items are in different branches, no correction is needed
				}

				if (*i > *j) {
					--(*i);
				}
				break;
			} else {
				if (j == std::prev(index.end())) {
					cur_iter_index = utki::make_vector(index);
					break;
				}
			}
		}
	}

	// TODO: this assert failed once when removing an item from test/app tree_view
	// to reproduce, try to unfold all items and them remove some item
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

	this->list_provider::notify_model_change();
}
