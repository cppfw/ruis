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

#include "dialog.hpp"

#include "../../../context.hpp"
#include "../../../layout/abstract_layout.hpp"
#include "../../label/padding.hpp"
#include "../../label/rectangle.hpp"
#include "../../proxy/click_proxy.hpp"
#include "../../proxy/key_proxy.hpp"
#include "../../proxy/mouse_proxy.hpp"

using namespace ruis;
using namespace ruis::touch;

namespace {

namespace m = ruis::make;

// Build the chrome (dimming background, close-on-click/keypress proxies and the styled panel)
// surrounding the given dialog content container.
widget_list make_chrome(
	utki::shared_ref<ruis::context> c, //
	utki::shared_ref<ruis::container> content_container, //
	dialog::all_parameters params, //
	std::function<void(ruis::click_proxy&)> bg_click_handler, //
	std::function<ruis::event_status(ruis::key_proxy&, const ruis::key_event&)> key_handler
)
{
	const auto& style = c.get().style();

	// Dimming background
	// clang-format off
	auto dim_bg = m::rectangle(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.color_params{
				.color = style.get_color_dimmed()
			}
		}
	);
	// clang-format on

	// Background click proxy to close the dialog when clicking outside of the panel
	// clang-format off
	auto bg_click_proxy = m::click_proxy(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.click_proxy_params{
				.click_handler = std::move(bg_click_handler)
			}
		}
	);
	// clang-format on

	// Key proxy to close the dialog on Escape key press
	// clang-format off
	auto key_proxy = m::key_proxy(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.key_proxy_params{
				.key_handler = std::move(key_handler)
			}
		},
		{}
	);
	// clang-format on

	// Dialog panel background
	// clang-format off
	auto panel_bg = m::rectangle(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.padding_params{
				.borders = [&](){
					for(auto& b : params.padding_params.borders){
						if(b.get().is_undefined()){
							b = style.get_len_dialog_padding();
						}
					}
					return std::move(params.padding_params.borders);
				}()
			},
			.color_params{
				.color = [&](){
					if(params.color_params.color.get().is_undefined()){
						return style.get_color_panel();
					}else{
						return std::move(params.color_params.color);
					}
				}()
			},
			.rectangle_params{
				.corner_radii = [&](){
					for(auto& r : params.rectangle_params.corner_radii){
						if(r.get().is_undefined()){
							r = style.get_len_dialog_padding();
						}
					}
					return std::move(params.rectangle_params.corner_radii);
				}()
			}
		},
		{
			std::move(content_container)
		}
	);
	// clang-format on

	// Dialog container with padding providing the margin around the panel
	// clang-format off
	auto margin_container = m::padding(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.padding_params{
				.borders = [&](){
					auto borders = params.dialog_params.margin_params.borders;
					for(auto& b : borders){
						if(b.get().is_undefined()){
							b = style.get_len_dialog_margin();
						}
					}
					return borders;
				}()
			}
		},
		{
			// Mouse proxy to consume mouse events inside the dialog,
			// so that clicks on the panel do not reach the background click proxy.
			m::mouse_proxy(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.mouse_proxy_params{
						.mouse_button_handler = [](auto&, auto&){ return ruis::event_status::consumed; },
						.mouse_move_handler = [](auto&, auto&){ return ruis::event_status::consumed; }
					}
				}
			),
			std::move(panel_bg)
		}
	);
	// clang-format on

	return {
		std::move(dim_bg), //
		std::move(bg_click_proxy), //
		std::move(key_proxy), //
		std::move(margin_container)
	};
}

} // namespace

dialog::dialog(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	content_wrapping(m::container(
		this->context,
		// clang-format off
		{
			.layout_params = {
				// content container fills the dialog panel, so that fill-sized children
				// (e.g. scrollable lists) can take the whole available area of the panel
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.container_params = [&](){
				if(!params.container_params.layout){
					params.container_params.layout = ruis::layout::pile;
				}
				return std::move(params.container_params);
			}()
		},
		// clang-format on
		std::move(children)
	)),
	// clang-format off
	container(
		this->context,
		{
			.container_params = {
				.layout = ruis::layout::pile
			}
		},
		make_chrome(
			this->context, //
			this->content_container, //
			params, //
			[this](ruis::click_proxy&){ this->close(); }, //
			[this](ruis::key_proxy&, const ruis::key_event& e) -> ruis::event_status {
				if (e.action == ruis::button_action::press && e.combo.key == ruis::key::escape) {
					this->close();
					return ruis::event_status::consumed;
				}
				return ruis::event_status::propagate;
			}
		)
	)
// clang-format on
{}

void dialog::close()
{
	auto self_weak = utki::make_weak_from(*this);
	this->context.get().post_to_ui_thread([self_weak]() {
		if (auto self = self_weak.lock()) {
			self->remove_from_parent();
		}
	});
}

utki::shared_ref<ruis::touch::dialog> ruis::touch::make::dialog(
	utki::shared_ref<context> context, //
	dialog::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::touch::dialog>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
