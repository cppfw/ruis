#pragma once

#include <morda/updateable.hpp>

#include "../../../src/morda/widgets/group/book.hpp"

class cube_page :
		public morda::page,
		public morda::container
{
	std::shared_ptr<morda::updateable> cube;
public:
	cube_page(const utki::shared_ref<morda::context>& c);
	
	cube_page(const cube_page&) = delete;
	cube_page& operator=(const cube_page&) = delete;

	cube_page(cube_page&&) = delete;
	cube_page& operator=(cube_page&&) = delete;

	~cube_page()override{
		std::cout << "cube_page destroyed" << std::endl;
	}
	
	void on_show() override;
	void on_hide()noexcept override;
	
private:

};

