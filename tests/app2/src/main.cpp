#include <ruisapp/application.hpp>

#include "gui.hpp"

class application : public ruisapp::application
{
public:
	application() :
		ruisapp::application("ruis-tests", []() {
			ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));

			return wp;
		}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file("res/"));

		this->gui.set_root(make_gui(this->gui.context));
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args) {
	return std::make_unique<::application>();
});
