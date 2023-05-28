#pragma once

#include "../../../src/morda/widgets/group/book.hpp"

class pile_page :
		public morda::page,
		public morda::container
{
public:
	pile_page(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);
	
	pile_page(const pile_page&) = delete;
	pile_page& operator=(const pile_page&) = delete;

	~pile_page()override{
		std::cout << "pile_page destroyed" << std::endl;
	}
	
private:

};
