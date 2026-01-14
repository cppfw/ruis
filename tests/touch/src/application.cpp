/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "application.hpp"

#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruisapp/application.hpp>

#include "gui.hpp"

application::application() :
	ruisapp::application({.name = "ruis-app-touch"}),
	window(this->make_window({.dims = r4::vector2<unsigned>(1116, 2484) / 3}))
{
	this->window.gui.context.get().window().close_handler = [this]() {
		this->quit();
	};

	this->window.gui.init_standard_widgets(this->get_res_file("../../res/ruis_res/").get());

	// TODO: add localization
	// this->window.gui.context.get().localization.get() =
	// 	ruis::localization(tml::read(this->get_res_file("res/localization/en.tml").get()));

	this->window.gui.context.get().loader().mount_res_pack(this->get_res_file("res/").get());

	// clang-format off
	auto kp = ruis::make::key_proxy(
		this->window.gui.context,
		{
			.container_params{
				.layout = ruis::layout::pile
			}
		},
		{
			make_root_widget(this->window.gui.context)
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
