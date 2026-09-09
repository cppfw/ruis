#include "pile_page.hpp"

#include <ruis/layout/pile_layout.hpp>

pile_page::pile_page(
	const utki::shared_ref<ruis::context>& context, //
	ruis::widget::parameters widget_params,
	ruis::widget_list contents
) :
	widget(context, {}, std::move(widget_params)),
	page(context, ruis::widget::parameters{}),
	container(
		context,//
		{
			.container_params{
				.layout = ruis::layout::pile
			}
		},
		std::move(contents)
	)
{}