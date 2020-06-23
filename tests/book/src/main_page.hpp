#pragma once

#include "../../../src/morda/morda/widgets/group/pile.hpp"
#include "../../../src/morda/morda/widgets/group/book.hpp"

class main_page :
		public morda::page,
		private morda::pile
{
public:
	main_page(std::shared_ptr<morda::context> c);
	
	main_page(const main_page&) = delete;
	main_page& operator=(const main_page&) = delete;
	
private:

};
