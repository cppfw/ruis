#pragma once

#include <morda/widgets/group/pile.hpp>
#include <morda/updateable.hpp>

#include "../../../src/morda/morda/widgets/group/book.hpp"

class cube_page :
		public morda::page,
		public morda::pile
{
	std::shared_ptr<morda::updateable> cube;
public:
	cube_page(std::shared_ptr<morda::context> c);
	
	cube_page(const cube_page&) = delete;
	cube_page& operator=(const cube_page&) = delete;
	
	void on_show() override;
	void on_hide() override;
	
private:

};

