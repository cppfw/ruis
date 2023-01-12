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

#pragma once

#include <unordered_map>

#include "book.hpp"
#include "column.hpp"
#include "../button/tab_group.hpp"
#include "../button/tab.hpp"

namespace morda{

class tabbed_book :
		virtual public morda::widget,
		private morda::column
{
	morda::tab_group& tab_group;
	morda::book& book;

	struct tab_page_pair{
		morda::tab* tab;
		morda::page* page;
	};

	std::vector<tab_page_pair> tab_page_pairs;

	decltype(tab_page_pairs)::iterator find_pair(const morda::tab& t);
	decltype(tab_page_pairs)::iterator find_pair(const morda::page& p);

	void activate_another_tab(tab& t);
public:
	tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc);

	void add(const utki::shared_ref<tab>& tab, const utki::shared_ref<morda::page>& page);

	const morda::book& get_book()const noexcept{
		return this->book;
	}

	std::shared_ptr<page> tear_out(tab& t);

	using morda::column::render;
	using morda::column::lay_out;
	using morda::column::on_mouse_move;
	using morda::column::on_mouse_button;
	using morda::column::on_key;
	using morda::column::on_focus_change;
	using morda::column::on_hover_change;
	using morda::column::on_resize;
	using morda::column::on_parent_change;
	using morda::column::measure;
	using morda::column::on_enable_change;
};

}
