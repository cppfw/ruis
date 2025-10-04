#include "application.hpp"

#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruisapp/application.hpp>

#include "gui.hpp"

application::application() :
	ruisapp::application({
		.name = "ruis-tests"
}),
	window(this->make_window({.dims = {1024, 800}}))
{
	this->window.gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

	this->window.gui.context.get().localization.get() =
		ruis::localization(tml::read(*this->get_res_file("res/localization/en.tml")));

	this->window.gui.context.get().loader().mount_res_pack(*this->get_res_file("res/"));

	// clang-format off
		auto kp = ruis::make::key_proxy(
			this->window.gui.context,
			{
				.container_params{
					.layout = ruis::layout::pile
				}
			},
			{
				make_root_widgets_structure(this->window.gui.context)
			}
		);
	// clang-format on

	kp.get().key_handler = [this](ruis::key_proxy&, const ruis::key_event& e) -> bool {
		if (e.is_down) {
			if (e.combo.key == ruis::key::escape) {
				this->quit();
			}
		}
		return false;
	};

	this->window.gui.set_root(std::move(kp));
}
