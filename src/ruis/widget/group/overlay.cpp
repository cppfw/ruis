/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "overlay.hpp"

#include <utki/config.hpp>

#include "../../context.hpp"
#include "../../layout/pile_layout.hpp"
#include "../../layout/size_layout.hpp"
#include "../container.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace ruis;

namespace {
class popup_wrapper : public container
{
public:
	popup_wrapper(
		utki::shared_ref<ruis::context> context, //
		widget_list children
	) :
		// clang-format off
		widget(
			std::move(context),
			{
				.dims{ruis::dim::fill, ruis::dim::fill}
			},
			{}
		),
		container(this->context,
			{
				.container_params{
					.layout = ruis::layout::size
				}
			},
			std::move(children)
		)
	// clang-format on
	{}

	static utki::shared_ref<popup_wrapper> make(
		utki::shared_ref<ruis::context> context, //
		widget_list children
	)
	{
		return utki::make_shared<popup_wrapper>(
			std::move(context), //
			std::move(children)
		);
	}
};
} // namespace

overlay::overlay(
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	container( //
		this->context,
		{
			.container_params{
				.layout = layout::pile
			}
		},
		std::move(children)
	)
// clang-format on
{}

utki::shared_ref<widget> overlay::show_popup(
	utki::shared_ref<widget> popup, //
	vector2 anchor
)
{
	// clang-format off
	auto c = popup_wrapper::make(this->context,
		{
			ruis::make::mouse_proxy(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.rectangle{
							{0, 0}, // set left top corner
							{1, 1} // dimensions do not matter
						}
					}
				}
			)
		}
	);
	// clang-format on

	auto mp = utki::dynamic_reference_cast<mouse_proxy>(c.get().children().back());

	mp.get().mouse_button_handler = //
		[cntr{utki::make_weak(c)}](
			mouse_proxy& w, //
			const mouse_button_event& e
		) -> bool //
	{
		if (auto c = cntr.lock()) {
			c->context.get().post_to_ui_thread([c]() {
				c->remove_from_parent();
			});
		}
		return false;
	};

	auto& w = popup.get();

	c.get().push_back(std::move(popup));

	vector2 dim = dims_for_widget(
		w, //
		this->rect().d
	);

	using std::min;
	using std::max;

	dim = min(dim, this->rect().d); // clamp top

	w.resize(dim);

	anchor = max(anchor, 0); // clamp bottom
	anchor = min(anchor, this->rect().d - w.rect().d); // clamp top

	w.move_to(anchor);

	auto sp = utki::make_shared_from(*this);
	this->context.get().post_to_ui_thread([c, sp]() {
		sp.get().push_back(c);
	});

	return c;
}

void overlay::close_all_popups()
{
	auto menus = this->get_all_widgets<popup_wrapper>();
	for (auto& w : menus) {
		w.get().remove_from_parent();
	}
}
