#include <utki/debug.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/morda/widgets/label/text.hpp"

#include "tabbed_book.hpp"

class application : public mordavokne::application{
public:
	application() :
			mordavokne::application(
					"morda-tests",
					[]() -> mordavokne::window_params {
						mordavokne::window_params wp(r4::vector2<unsigned>(640, 480));

						return wp;
					}()
				)
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		// this->gui.context->loader.mount_res_pack(*this->get_res_file("res/"));

		this->gui.context->inflater.register_widget<morda::tabbed_book>("tabbed_book");

		std::shared_ptr<morda::widget> c = this->gui.context->inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		// auto& book = c->get_widget_as<morda::tabbed_book>("book");
	}
};

mordavokne::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
