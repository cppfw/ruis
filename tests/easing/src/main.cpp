#include <ruis/standard_resources.hpp>
#include <ruisapp/application.hpp>

#include "gui.hpp"

class application : public ruisapp::application
{
	ruisapp::window& window;

public:
	application() :
		ruisapp::application({
			.name = "ruis-tests",
			.mount_ruis_res_pack = false
    }),
		window(this->make_window({.dims = {1024, 800}}))
	{
		this->window.gui.mount_ruis_res_pack(this->get_res_file("../../res/ruis_res/"));

		// this->gui.context.get().loader.mount_res_pack(this->get_res_file("res/"));

		this->window.gui.set_root(make_gui(this->window.gui.context));

		this->window.gui.default_key_handler = [this](const ruis::key_event& e) {
			if (e.action == ruis::button_action::press) {
				if (e.combo.key == ruis::key::escape) {
					this->quit();
				}
			}
		};

		this->window.gui.context.get().window().close_handler = [this]() {
			this->quit();
		};
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args) {
	return std::make_unique<::application>();
});
