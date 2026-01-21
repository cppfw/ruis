/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "tabbed_book.hpp"

using namespace std::string_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

tabbed_book::tabbed_book(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	pages_list_type pages
) :
	tabbed_book(
		context,
		params,
		pages,
		[&]() {
			widget_list children;
			children.reserve(pages.size());
			for (const auto& p : pages) {
				children.emplace_back(p.first);
			}

			if (!params.tabbed_book_params.choice_group_factory) {
				// clang-format off
				params.tabbed_book_params.choice_group_factory = [](
						utki::shared_ref<ruis::context> context,
						widget_list tabs
					){
						return ruis::make::choice_group(std::move(context),
							{
								.layout_params{
									.dims{ruis::dim::fill, ruis::dim::min}
								},
								.container_params{
									.layout = ruis::layout::row
								}
							},
							std::move(tabs)
						);
					};
				// clang-format on
			}
			return params.tabbed_book_params.choice_group_factory(
				context, //
				std::move(children)
			);
		}(),
		[&]() {
			std::vector<utki::shared_ref<page>> children;
			children.reserve(pages.size());
			for (const auto& p : pages) {
				children.emplace_back(p.second);
			}

			if (!params.tabbed_book_params.book_factory) {
				// clang-format off
				params.tabbed_book_params.book_factory = [](
						utki::shared_ref<ruis::context> context, //
						std::vector<utki::shared_ref<page>> pages
					){
						return ruis::make::book(std::move(context),
							{
								.layout_params{
									.dims{ruis::dim::fill, ruis::dim::max},
									.weight = 1
								}
							},
							std::move(pages)
						);
					};
				// clang-format on
			}
			return params.tabbed_book_params.book_factory(
				context, //
				std::move(children)
			);
		}()
	)
{}

tabbed_book::tabbed_book(
	utki::shared_ref<ruis::context> context, //
	all_parameters& params,
	pages_list_type& pages,
	utki::shared_ref<ruis::choice_group> choice_group,
	utki::shared_ref<ruis::book> book
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	container(this->context,
		{
			.container_params{
				.layout = ruis::layout::column // TODO: get from params
			}
		},
		params.tabbed_book_params.reverse_order ? widget_list{
			book,
			choice_group
		} : widget_list{
			choice_group,
			book
		}
	),
	// clang-format on
	choice_group(choice_group),
	book(book), //
	tab_page_pairs(std::move(pages))
{
	for (auto& pair : this->tab_page_pairs) {
		this->set_tab_pressed_change_handler(pair);
	}

	// on page tear out, remove corresponding tab
	this->book.pages_change_handler = [this](ruis::book& b, const page& p) {
		auto i = this->find_pair(p);
		if (i != this->tab_page_pairs.end()) {
			auto& [tab, page] = *i;
			this->activate_another_tab(tab);
			tab.get().remove_from_parent();
			this->tab_page_pairs.erase(i);
		}
	};

	// on page programmatic activate we need to activate the corresponding tab as well
	this->book.active_page_change_handler = [this](ruis::book& b) {
		utki::assert(b.get_active_page(), SL);
		auto i = this->find_pair(*b.get_active_page());
		if (i != this->tab_page_pairs.end()) {
			auto& [tab, page] = *i;
			tab.get().activate();
		}
	};

	if (!this->tab_page_pairs.empty()) {
		this->tab_page_pairs.back().first.get().activate();
	}
}

void tabbed_book::add(
	utki::shared_ref<choice_button> tab, //
	utki::shared_ref<ruis::page> page
)
{
	// TODO: what if exception is thrown on any step below?

	this->choice_group.push_back(tab);
	this->book.push(page);

	// make the tab active
	tab.get().set_pressed(true);

	this->tab_page_pairs.emplace_back(
		std::move(tab), //
		std::move(page)
	);

	this->set_tab_pressed_change_handler(this->tab_page_pairs.back());
}

void tabbed_book::set_tab_pressed_change_handler(typename decltype(tab_page_pairs)::value_type& pair)
{
	auto& [tab, page] = pair;

	tab.get().pressed_change_handler = [p = page](ruis::button& btn) {
		if (btn.is_pressed()) {
			p.get().activate();
		}
	};
}

void tabbed_book::activate_another_tab(choice_button& t)
{
	if (!t.is_pressed()) {
		return;
	}

	// find another tab and activate it
	auto i = this->choice_group.find(t);
	utki::assert(i != this->choice_group.end(), SL);
	utki::assert(!this->choice_group.empty(), SL);
	if (i == this->choice_group.begin()) {
		auto ni = std::next(i);
		if (ni != this->choice_group.end()) {
			auto next_tab = std::dynamic_pointer_cast<ruis::choice_button>(ni->to_shared_ptr());
			utki::assert(next_tab, SL);
			next_tab->set_pressed(true);
		}
	} else {
		utki::assert(i != this->choice_group.begin(), SL);
		auto ni = std::prev(i);
		utki::assert(ni >= this->choice_group.begin(), SL);
		auto next_tab = std::dynamic_pointer_cast<ruis::choice_button>(ni->to_shared_ptr());
		utki::assert(next_tab, SL);
		next_tab->set_pressed(true);
	}
}

utki::shared_ref<page> tabbed_book::tear_out(choice_button& t)
{
	auto i = this->find_pair(t);
	if (i == this->tab_page_pairs.end()) {
		throw std::logic_error("tabbed_book::tear_out(): tab not found");
	}

	auto page = i->second;

	this->tab_page_pairs.erase(i);

	utki::assert(t.parent() == &this->choice_group, SL);

	this->activate_another_tab(t);

	t.remove_from_parent();

	utki::assert(!page.get().is_active() || this->book.size() == 1, SL);
	page.get().tear_out();

	return page;
}

auto tabbed_book::find_pair(const ruis::choice_button& t) -> decltype(tab_page_pairs)::iterator
{
	return std::find_if(
		this->tab_page_pairs.begin(), //
		this->tab_page_pairs.end(),
		[&t](const auto& pair) {
			return &t == &pair.first.get();
		}
	);
}

auto tabbed_book::find_pair(const ruis::page& p) -> decltype(tab_page_pairs)::iterator
{
	return std::find_if(
		this->tab_page_pairs.begin(), //
		this->tab_page_pairs.end(),
		[&p](const auto& pair) {
			return &p == &pair.second.get();
		}
	);
}

utki::shared_ref<ruis::tabbed_book> ruis::make::tabbed_book(
	utki::shared_ref<ruis::context> context, //
	ruis::tabbed_book::all_parameters params,
	std::vector< //
		std::pair<
			utki::shared_ref<ruis::choice_button>, //
			utki::shared_ref<ruis::page> //
			> //
		> //
		pages
)
{
	if (!params.tabbed_book_params.choice_group_factory) {
		// clang-format off
		params.tabbed_book_params.choice_group_factory = [](
				utki::shared_ref<ruis::context> context,
				widget_list tabs
			){
				return ruis::make::tab_group(std::move(context),
					{
						.layout_params{
							.dims{ruis::dim::fill, ruis::dim::min}
						}
					},
					std::move(tabs)
				);
			};
		// clang-format on
	}

	return utki::make_shared<ruis::tabbed_book>(
		std::move(context), //
		std::move(params),
		std::move(pages)
	);
}
