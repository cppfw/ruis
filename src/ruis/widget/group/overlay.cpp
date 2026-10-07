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

#include "overlay.hpp"

#include <utki/config.hpp>

#include "../container.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace ruis;

namespace {
class popup_wrapper : public container
{
	// full-screen click catcher, closes the popup on the first press and consumes the events
	// so that they do not propagate to the widgets below the popup (to prevent accidental
	// clicks on them)
	utki::shared_ref<mouse_proxy> click_catcher;

public:
	popup_wrapper(const utki::shared_ref<ruis::context>& context) :
		// clang-format off
		widget(
			context,
			{
				.dims{ruis::dim::fill, ruis::dim::fill}
			},
			{}
		),
		container(context,
			{
				.params{
					.layout = ruis::layout::size
				}
			},
			{}
		),
		click_catcher([&]() {
			// clang-format off
			return ruis::make::mouse_proxy(context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget{
						.rectangle{
							{0, 0}, // set left top corner
							{1, 1} // dimensions do not matter
						}
					}
				}
			);
			// clang-format on
		}())
	// clang-format on
	{
		// the handler is owned by the click_catcher which is a member of this wrapper,
		// so the handler cannot outlive the wrapper and it is safe to use a raw
		// reference to the wrapper in the handler
		this->click_catcher.get().mouse_button_handler = //
			[ //
				self{this}, //
				removal_requested{false} //
		]( //
				mouse_proxy& w, //
				const mouse_button_event& e
			) mutable //
		{
			// The removal is posted to the ui thread, so it may happen that several press events
			// are received before the popup is actually removed from the parent; the flag makes sure
			// that the removal is requested only once.
			// Close the popup on the first press and request the removal only once
			if (!removal_requested && e.action == button_action::press) {
				removal_requested = true;
				self->ctx().post_to_ui_thread([self]() {
					self->remove_from_parent();
				});
			}
			// consume the event so that it does not propagate to the widgets
			// below the popup (to prevent accidental clicks on them);
			// any event after the removal request (e.g. the corresponding release)
			// is also just consumed and ignored
			return event_status::consumed;
		};

		this->push_back(this->click_catcher);
	}

	void on_children_change() override
	{
		// The wrapper always contains the click_catcher and the popup contents;
		// in case the popup contents are removed (e.g. a context menu closes itself
		// when a menu item is clicked), remove the wrapper too, so that its full-screen
		// click catcher does not remain on top of the other widgets.
		// (if the wrapper is not attached to a parent yet, there is nothing to remove,
		// this also makes sure that make_shared_from is not called in the constructor)
		if (this->parent() && this->children().size() < 2) {
			auto self = utki::make_shared_from(*this);
			this->context.get().post_to_ui_thread([self]() {
				if (self.get().parent()) {
					self.get().remove_from_parent();
				}
			});
		}
	}
};

} // namespace

overlay::overlay(
	const utki::shared_ref<ruis::context>& context,
	all_parameters params,
	widget_list children
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// clang-format off
	container( //
		context,
		{
			.params{
				.layout = layout::pile
			}
		},
		std::move(children)
	)
// clang-format on
{}

utki::shared_ref<widget> overlay::show_popup(
	utki::shared_ref<widget> popup, //
	vec2 anchor
)
{
	auto c = utki::make_shared<popup_wrapper>(this->context);

	auto& w = popup.get();

	c.get().push_back(std::move(popup));

	vec2 dim = w.measure_within_parent(
		this->rect().d, //
		r4::vector2<measure_mode>(measure_mode::exactly)
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
