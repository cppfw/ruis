#pragma once

#include <ruis/updateable.hpp>
#include <ruis/widget/group/book.hpp>

class cube_page :
		public ruis::page,
		public ruis::container
{
	std::shared_ptr<ruis::updateable> cube;
public:
	cube_page(const utki::shared_ref<ruis::context>& c);
	
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

