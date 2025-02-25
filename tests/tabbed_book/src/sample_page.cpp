#include "sample_page.hpp"

#include <ruis/widget/label/text.hpp>
#include <ruis/widget/button/push_button.hpp>

using namespace std::string_literals;

sample_page::sample_page(
	utki::shared_ref<ruis::context> context, //
	std::string_view text
) :
	ruis::widget(std::move( context), {}, {}),
	ruis::page(this->context, {}),
	// clang-format off
	ruis::container(
		this->context,
		{
			.container_params{
				.layout = ruis::layout::row
			}
		},
		{
			ruis::make::text(this->context,
				{
					.widget_params{
						.id = "text"s
					}
				}
			),
			ruis::make::push_button(this->context,
				{
					.widget_params{
						.id = "button"s
					}
				},
				{
					ruis::make::text(this->context,
						{},
						U"tear out the page"s
					)
				}
			)
		}
	)
	// clang-format on
{
	this->get_widget_as<ruis::text>("text").set_text(text);

	this->get_widget_as<ruis::push_button>("button").click_handler = [this](ruis::push_button&){
		this->context.get().post_to_ui_thread([pg = utki::make_shared_from(*this)]{
			pg.get().tear_out();
		});
	};
}
