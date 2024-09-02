#include <ruis/widget/proxy/key_proxy.hpp>
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

		auto kp = ruis::make::key_proxy(
			this->gui.context,
			{.container_params = {.layout = ruis::layout::pile}},
			{make_root_widgets_structure(this->gui.context)}
		);

		kp.get().key_handler = [this](ruis::key_proxy&, const ruis::key_event& e) -> bool {
			if (e.is_down) {
				if (e.combo.key == ruis::key::escape) {
					this->quit();
				}
			}
			return false;
		};

		this->gui.set_root(std::move(kp));
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args) {
	return std::make_unique<::application>();
});
