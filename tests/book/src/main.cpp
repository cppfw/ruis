#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/morda/widgets/label/busy.hpp"
#include "../../../src/morda/morda/widgets/group/book.hpp"

#include "pile_page.hpp"
#include "cube_page.hpp"

class application : public mordavokne::application{
	static mordavokne::window_params GetWindowParams()noexcept{
		mordavokne::window_params wp(r4::vector2<unsigned>(640, 480));

		return wp;
	}
public:
	application() :
			mordavokne::application("morda-tests", GetWindowParams())
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		this->gui.context->loader.mount_res_pack(*this->get_res_file("res/"));

		std::shared_ptr<morda::widget> c = this->gui.context->inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		auto& b = c->get_widget_as<morda::book>("book");

		{
			auto mp = std::make_shared<pile_page>(
					this->gui.context,
					treeml::read(R"qwertyuiop(
							@column{
								layout{dx{fill} dy{fill}}

								@push_button{
									id{cube_button}

									layout{
										dx{fill}
									}

									@text{
										text{Cube!}
									}
								}

								@push_button{
									id{stuff_button}

									layout{
										dx{fill}
									}

									@text{
										text{Stuff!}
									}
								}
							}
						)qwertyuiop")
				);
			mp->get_widget_as<morda::push_button>("cube_button").click_handler = [mp](morda::push_button& b){
				mp->get_parent_book().push(std::make_shared<cube_page>(mp->context));
			};
			mp->get_widget_as<morda::push_button>("stuff_button").click_handler = [mp](morda::push_button& b){
				auto pg = std::make_shared<pile_page>(mp->context, treeml::read(R"qwertyuiop(
					@push_button{
						id{back_button}
						@text{
							text{"Go back"}
						}
					}
				)qwertyuiop"));
				auto pg_ptr = pg.get();
				pg->get_widget_as<morda::push_button>("back_button").click_handler = [mp, pg_ptr](morda::push_button& b){
					pg_ptr->tear_out();
				};
				mp->get_parent_book().push(pg);
			};
			b.push(mp);
		}
	}
};

std::unique_ptr<mordavokne::application> mordavokne::create_application(int argc, const char** argv){
	return std::make_unique<::application>();
}
