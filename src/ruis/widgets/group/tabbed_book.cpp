/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

using namespace ruis;

tabbed_book::tabbed_book(const utki::shared_ref<ruis::context>& context, const tml::forest& desc) :
	ruis::widget(context, desc),
	ruis::container(this->context, tml::read(R"(
					layout{column}
					
					@tab_group{
						id { ruis_tab_group }
						lp{
							dx { fill }
						}
					}
					@book{
						id { ruis_book }
						lp {
							dx { fill }
							dy { max }
							weight { 1 }
						}
					}
				)")),
	tab_group(this->get_widget_as<ruis::tab_group>("ruis_tab_group")),
	book(this->get_widget_as<ruis::book>("ruis_book"))
{
	// on page tear out, remove corresponding tab
	this->book.pages_change_handler = [this](ruis::book& b, const page& p) {
		auto i = this->find_pair(p);
		if (i != this->tab_page_pairs.end()) {
			ASSERT(i->tab)
			this->activate_another_tab(*i->tab);
			i->tab->remove_from_parent();
			this->tab_page_pairs.erase(i);
		}
	};

	// on page programmatic activate we need to activate the corresponding tab as well
	this->book.active_page_change_handler = [this](ruis::book& b) {
		ASSERT(b.get_active_page())
		auto i = this->find_pair(*b.get_active_page());
		if (i != this->tab_page_pairs.end()) {
			ASSERT(i->tab)
			i->tab->activate();
		}
	};
}

void tabbed_book::add(const utki::shared_ref<tab>& tab, const utki::shared_ref<ruis::page>& page)
{
	this->tab_group.push_back(tab);
	this->book.push(page);

	tab.get().set_pressed(true);

	tab.get().pressed_change_handler = [page](ruis::button& btn) {
		if (btn.is_pressed()) {
			page.get().activate();
		}
	};

	this->tab_page_pairs.push_back(tab_page_pair{&tab.get(), &page.get()});
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

	ASSERT(i->page)
	auto pg = utki::make_shared_from(*i->page);

	this->tab_page_pairs.erase(i);

	ASSERT(t.parent() == &this->tab_group)

	this->activate_another_tab(t);

	t.remove_from_parent();

	ASSERT(!pg.get().is_active() || this->book.size() == 1)
	pg.get().tear_out();

	return pg;
}

auto tabbed_book::find_pair(const ruis::tab& t) -> decltype(tab_page_pairs)::iterator
{
	return std::find_if(this->tab_page_pairs.begin(), this->tab_page_pairs.end(), [&t](const auto& e) {
		return &t == e.tab;
	});
}

auto tabbed_book::find_pair(const ruis::page& p) -> decltype(tab_page_pairs)::iterator
{
	return std::find_if(this->tab_page_pairs.begin(), this->tab_page_pairs.end(), [&p](const auto& e) {
		return &p == e.page;
	});
}
