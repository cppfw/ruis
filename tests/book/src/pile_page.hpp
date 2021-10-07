#pragma once

#include "../../../src/morda/widgets/group/pile.hpp"
#include "../../../src/morda/widgets/group/book.hpp"

class pile_page :
		public morda::page,
		public morda::pile
{
public:
	pile_page(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	pile_page(const pile_page&) = delete;
	pile_page& operator=(const pile_page&) = delete;
	
private:

};
