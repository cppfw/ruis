#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widgets/button/base/push_button.hpp>
#include <ruis/widgets/label/busy.hpp>
#include <ruis/widgets/group/book.hpp>
#include <ruis/widgets/button/tab_group.hpp>
#include <ruis/widgets/button/tab.hpp>
#include <ruis/widgets/label/text.hpp>

#include "pile_page.hpp"
#include "cube_page.hpp"

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application("ruis-tests", [](){
				ruisapp::window_params wp(r4::vector2<unsigned>(640, 480));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file("res/"));

		auto c = this->gui.context.get().inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		auto& book = c.get().get_widget_as<ruis::book>("book");

		{
			auto mp = utki::make_shared<pile_page>(
					this->gui.context,
					tml::read(R"qwertyuiop(
							@column{
								lp{dx{fill} dy{fill}}

								@push_button{
									id{cube_button}

									lp{
										dx{fill}
									}

									@text{
										text{Cube!}
									}
								}

								@push_button{
									id{stuff_button}

									lp{
										dx{fill}
									}

									@text{
										text{Stuff!}
									}
								}

								@push_button{
									id{close_button}

									lp{
										dx{fill}
									}

									@text{
										text{close}
									}
								}
							}
						)qwertyuiop")
				);
			mp.get().get_widget_as<ruis::push_button>("cube_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				mp.get_parent_book()->push(utki::make_shared<cube_page>(mp.context));
			};
			mp.get().get_widget_as<ruis::push_button>("stuff_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				auto pg = utki::make_shared<pile_page>(mp.context, tml::read(R"qwertyuiop(
					@push_button{
						id{back_button}
						@text{
							text{"Go back"}
						}
					}
				)qwertyuiop"));
				pg.get().get_widget_as<ruis::push_button>("back_button").click_handler = [&pg = pg.get()](ruis::push_button& b){
					b.context.get().run_from_ui_thread([pg = utki::make_shared_from(pg)]{
						pg.get().tear_out();
					});
				};
				mp.get_parent_book()->push(pg);
			};
			mp.get().get_widget_as<ruis::push_button>("close_button").click_handler = [&mp = mp.get()](ruis::push_button& b){
				b.context.get().run_from_ui_thread([pg = utki::make_shared_from(mp)]{
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
