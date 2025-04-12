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

#include "tabbed_book.hpp"

#include "../button/tab.hpp"
#include "../button/tab_group.hpp"

using namespace std::string_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

tabbed_book::tabbed_book(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	std::vector< //
		std::pair<
			utki::shared_ref<tab>, //
			utki::shared_ref<page> //
			> //
		> //
		pages
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	container(this->context,
		{
			.container_params{
				.layout = ruis::layout::column
			}
		},
		{
			m::tab_group(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::min}
					},
					.widget_params{
						.id = "ruis_tab_group"s
					}
				},
				[&](){
					std::vector<utki::shared_ref<widget>> children;
					children.reserve(pages.size());
					for(const auto& p : pages){
						children.emplace_back(p.first);
					}
					return children;
				}()
			),
			m::book(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::max},
						.weight = 1
					},
					.widget_params{
						.id = "ruis_book"s
					}
				},
				[&](){
					std::vector<utki::shared_ref<page>> children;
					children.reserve(pages.size());
					for(const auto& p : pages){
						children.push_back(p.second);
					}
					return children;
				}()
			)
		}
	),
	// clang-format on
	tab_group(this->get_widget_as<ruis::tab_group>("ruis_tab_group")),
	book(this->get_widget_as<ruis::book>("ruis_book")), //
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
		ASSERT(b.get_active_page())
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
	utki::shared_ref<tab> tab, //
	utki::shared_ref<ruis::page> page
)
{
	// TODO: what if exception is thrown on any step below?

	this->tab_group.push_back(tab);
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

void tabbed_book::activate_another_tab(tab& t)
{
	if (!t.is_pressed()) {
		return;
	}

	// find another tab and activate it
	auto i = this->tab_group.find(t);
	ASSERT(i != this->tab_group.end())
	ASSERT(!this->tab_group.empty())
	if (i == this->tab_group.begin()) {
		auto ni = std::next(i);
		if (ni != this->tab_group.end()) {
			auto next_tab = std::dynamic_pointer_cast<ruis::tab>(ni->to_shared_ptr());
			ASSERT(next_tab)
			next_tab->set_pressed(true);
		}
	} else {
		ASSERT(i != this->tab_group.begin())
		auto ni = std::prev(i);
		ASSERT(ni >= this->tab_group.begin())
		auto next_tab = std::dynamic_pointer_cast<ruis::tab>(ni->to_shared_ptr());
		ASSERT(next_tab)
		next_tab->set_pressed(true);
	}
}

utki::shared_ref<page> tabbed_book::tear_out(tab& t)
{
	auto i = this->find_pair(t);
	if (i == this->tab_page_pairs.end()) {
		throw std::logic_error("tabbed_book::tear_out(): tab not found");
	}

	auto page = i->second;

	this->tab_page_pairs.erase(i);

	ASSERT(t.parent() == &this->tab_group)

	this->activate_another_tab(t);

	t.remove_from_parent();

	ASSERT(!page.get().is_active() || this->book.size() == 1)
	page.get().tear_out();

	return page;
}

auto tabbed_book::find_pair(const ruis::tab& t) -> decltype(tab_page_pairs)::iterator
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
