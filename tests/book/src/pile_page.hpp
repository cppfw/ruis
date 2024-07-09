#pragma once

#include <ruis/widget/group/book.hpp>

class pile_page :
		public ruis::page,
		public ruis::container
{
public:
	pile_page(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);
	
	pile_page(const pile_page&) = delete;
	pile_page& operator=(const pile_page&) = delete;

	pile_page(pile_page&&) = delete;
	pile_page& operator=(pile_page&&) = delete;

	~pile_page()override{
		std::cout << "pile_page destroyed" << std::endl;
	}
	
private:

};
