#include "pile_page.hpp"

#include <ruis/layout/pile_layout.hpp>

pile_page::pile_page(
	utki::shared_ref<ruis::context> context, //
	ruis::widget::parameters widget_params,
	ruis::widget_list contents
) :
	widget(std::move(context), {}, std::move(widget_params)),
	page(this->context, ruis::widget::parameters{}),
	container(
		this->context,//
		{
			.container_params{
				.layout = ruis::layout::pile
			}
		},
		std::move(contents)
	)
{}