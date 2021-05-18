#pragma once

#include <morda/widgets/group/book.hpp>
#include <morda/widgets/group/column.hpp>
#include <morda/widgets/button/tab_group.hpp>

namespace morda{

class tabbed_book :
		virtual public morda::widget,
		private morda::column
{
	morda::tab_group& tab_group;
	morda::book& book;
public:
	tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc);

	void add(const std::string& name, std::shared_ptr<morda::page> page);
};

}
