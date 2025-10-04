#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/busy.hpp>
#include <ruis/widget/group/book.hpp>
#include <ruis/widget/button/tab_group.hpp>
#include <ruis/widget/button/tab.hpp>
#include <ruis/widget/label/text.hpp>

#include "pile_page.hpp"
#include "cube_page.hpp"

#include "main_page.hpp"

using namespace std::string_literals;

namespace m{
    using namespace ruis::make;
    using namespace ::make;
}

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application({
				.name = "ruis-tests"
			}),
			window(this->make_window(
				{
					.dims = {640, 480}
				}
			))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		this->window.gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->window.gui.context.get().loader().mount_res_pack(*this->get_res_file("res/"));

		auto& ctx = this->window.gui.context;

		// clang-format off
		auto c = m::column(ctx,
			{},
			{
				m::book(ctx,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::max},
							.weight = 1
						},
						.widget_params{
							.id = "book"s
						}
					}
				)
			}
		);
		// clang-format on

		this->window.gui.set_root(c);

		auto& book = c.get().get_widget_as<ruis::book>("book");

		{
			auto mp = make_main_page(ctx);
					
			mp.get().get_widget_as<ruis::push_button>("cube_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				mp.get_parent_book()->push(utki::make_shared<cube_page>(mp.context));
			};
			mp.get().get_widget_as<ruis::push_button>("stuff_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				// clang-format off
				auto pg = make::pile_page(
					mp.context,
					{},
					{
						m::push_button(mp.context,
							{
								.widget_params{
									.id = "back_button"s
								}
							},
							{
								m::text(mp.context,
									{},
									U"Go back"s
								)
							}
						)
					}
				);
				// clang-format on
				
				pg.get().get_widget_as<ruis::push_button>("back_button").click_handler = [&pg = pg.get()](ruis::push_button& b){
					b.context.get().post_to_ui_thread([pg = utki::make_shared_from(pg)]{
						pg.get().tear_out();
					});
				};
				mp.get_parent_book()->push(pg);
			};
			mp.get().get_widget_as<ruis::push_button>("close_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				b.context.get().post_to_ui_thread([pg = utki::make_shared_from(mp)]{
					pg.get().tear_out();
				});
			};
			book.push(mp);
		}
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
