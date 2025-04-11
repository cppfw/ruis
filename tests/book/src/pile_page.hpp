#pragma once

#include <ruis/widget/group/book.hpp>

class pile_page :
		public ruis::page,
		public ruis::container
{
public:
	pile_page( //
		utki::shared_ref<ruis::context> context,
		ruis::widget::parameters widget_params,
		ruis::widget_list contents
	);

	pile_page(const pile_page&) = delete;
	pile_page& operator=(const pile_page&) = delete;

	pile_page(pile_page&&) = delete;
	pile_page& operator=(pile_page&&) = delete;

	~pile_page()override{
		std::cout << "pile_page destroyed" << std::endl;
	}
	
private:

};

namespace make{
inline utki::shared_ref<::pile_page> pile_page(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::widget_list contents = {}
)
{
	return utki::make_shared<::pile_page>( //
		std::move(context),
		std::move(widget_params),
		std::move(contents)
	);
}
}