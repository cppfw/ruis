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

	std::unordered_map<tab*, page*> tab_to_page_map;

	void activate_another_tab(tab& t);
public:
	tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc);

	void add(std::shared_ptr<tab> tab, std::shared_ptr<morda::page> page);

	std::shared_ptr<page> tear_out(tab& t);
};

}
