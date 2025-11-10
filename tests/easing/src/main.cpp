#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruisapp/application.hpp>

#include "gui.hpp"

class application : public ruisapp::application
{
	ruisapp::window& window;

public:
	application() :
		ruisapp::application({
			.name = "ruis-tests",
    }),
		window(this->make_window({.dims = {1024, 800}}))
	{
		this->window.gui.init_standard_widgets(this->get_res_file("../../res/ruis_res/"));

		// this->gui.context.get().loader.mount_res_pack(this->get_res_file("res/"));

		auto kp = ruis::make::key_proxy(
			this->window.gui.context,
			{.container_params = {.layout = ruis::layout::pile}},
			{make_gui(this->window.gui.context)}
		);

		kp.get().key_handler = [this](ruis::key_proxy&, const ruis::key_event& e) -> bool {
			if (e.is_down) {
				if (e.combo.key == ruis::key::escape) {
					this->quit();
				}
			}
			return false;
		};

		this->window.gui.set_root(std::move(kp));

		this->window.gui.context.get().window().close_handler = [this]() {
			this->quit();
		};
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args) {
	return std::make_unique<::application>();
});
