/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

using namespace morda;

tabbed_book::tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc) :
		morda::widget(std::move(context), desc),
		morda::column(
				this->context,
				treeml::read(R"(
					@tab_group{
						id { morda_tab_group }
						layout{
							dx { fill }
						}
					}
					@book{
						id { morda_book }
						layout {
							dx { fill }
							weight { 1 }
						}
					}
				)")
			),
		tab_group(this->get_widget_as<morda::tab_group>("morda_tab_group")),
		book(this->get_widget_as<morda::book>("morda_book"))
{
	// TODO: set book::pages_change_handler to remove the tab
	// TODO: need to prohibit somehow tearing out of the page without removintg the tab
}

void tabbed_book::add(std::shared_ptr<tab> tab, std::shared_ptr<morda::page> page){
	this->tab_group.push_back(tab);
	this->book.push(page);

	tab->set_pressed(true);

	tab->press_handler = [page](morda::button& btn){
		if(btn.is_pressed()){
			page->activate();
		}
	};

	this->tab_to_page_map.insert(std::make_pair(tab.get(), page.get()));
}

std::shared_ptr<page> tabbed_book::tear_out(tab& t){
	auto i = this->tab_to_page_map.find(&t);
	if(i == this->tab_to_page_map.end()){
		throw std::logic_error("tabbed_book::tear_out(): tab not found");
	}

	auto pg = utki::make_shared_from(*i->second);

	this->tab_to_page_map.erase(i);

	ASSERT(t.parent() == &this->tab_group)

	// if the tab is currently active we need to switch to another tab before removing
	if(t.is_pressed()){
		// find previous/next tab and activate it
		auto i = this->tab_group.find(t);
		ASSERT(i != this->tab_group.end())
		ASSERT(!this->tab_group.empty())
		if(i == this->tab_group.begin()){
			auto ni = std::next(i);
			if(ni != this->tab_group.end()){
				auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
				ASSERT(next_tab)
				next_tab->set_pressed(true);
			}
		}else{
			ASSERT(i != this->tab_group.begin())
			auto ni = std::prev(i);
			ASSERT(ni >= this->tab_group.begin())
			auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
			ASSERT(next_tab)
			next_tab->set_pressed(true);
		}
	}

	this->context->run_from_ui_thread([t = utki::make_shared_from(t)](){
		t->remove_from_parent();
	});

	ASSERT(!pg->is_active() || this->book.size() == 1)
	pg->tear_out();

	return pg;
}
