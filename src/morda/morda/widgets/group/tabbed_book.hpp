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

	std::unordered_map<const tab*, page*> tab_to_page_map;
public:
	tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc);

	void add(std::shared_ptr<tab> tab, std::shared_ptr<morda::page> page);

	std::shared_ptr<page> tear_out(tab& t);
};

}
