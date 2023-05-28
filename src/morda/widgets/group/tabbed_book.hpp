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

#include "../button/tab.hpp"
#include "../button/tab_group.hpp"

#include "book.hpp"

namespace morda {

class tabbed_book : virtual public morda::widget, private morda::container
{
	morda::tab_group& tab_group;
	morda::book& book;

	struct tab_page_pair {
		morda::tab* tab;
		morda::page* page;
	};

	std::vector<tab_page_pair> tab_page_pairs;

	decltype(tab_page_pairs)::iterator find_pair(const morda::tab& t);
	decltype(tab_page_pairs)::iterator find_pair(const morda::page& p);

	void activate_another_tab(tab& t);

public:
	tabbed_book(const utki::shared_ref<morda::context>& context, const treeml::forest& desc);

	void add(const utki::shared_ref<tab>& tab, const utki::shared_ref<morda::page>& page);

	const morda::book& get_book() const noexcept
	{
		return this->book;
	}

	utki::shared_ref<page> tear_out(tab& t);

	using morda::container::render;
	using morda::container::on_lay_out;
	using morda::container::on_mouse_move;
	using morda::container::on_mouse_button;
	using morda::container::on_key;
	using morda::container::on_focus_change;
	using morda::container::on_hover_change;
	using morda::container::on_resize;
	using morda::container::on_parent_change;
	using morda::container::measure;
	using morda::container::on_enable_change;
};

} // namespace morda
