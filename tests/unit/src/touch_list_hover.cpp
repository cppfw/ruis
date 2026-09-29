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

#include <ruis/util/events.hpp>
#include <ruis/util/length.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/touch/list.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace {

/**
 * @brief A ruis::list_provider which provides a single item
 * containing a ruis::mouse_proxy.
 * This mimics the structure of the context menu items
 * (see tests/touch/src/context_menu.cpp).
 */
class hover_provider : public ruis::list_provider
{
public:
	utki::shared_ref<ruis::mouse_proxy> proxy;
	utki::shared_ref<ruis::container> item;

	hover_provider(const utki::shared_ref<ruis::context>& context) :
		ruis::list_provider(context), //
		// clang-format off
		proxy(ruis::make::mouse_proxy(context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim(ruis::length::make_pp(50))}
				}
			}
		)),
		// clang-format on
		// clang-format off
		item(ruis::make::pile(context,
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::min}
				}
			},
			{
				this->proxy
			}
		))
	// clang-format on
	{}

	size_t count() const noexcept override
	{
		return 1;
	}

	utki::shared_ref<ruis::widget> get_widget(size_t) override
	{
		return this->item;
	}
};

// Creates a ruis::touch::list with a single item from the given provider,
// resizes it to 100x100 pp and performs layouting.
utki::shared_ref<ruis::touch::list> make_laid_out_list(
	const utki::shared_ref<ruis::context>& c, //
	utki::unique_ref<hover_provider> provider
)
{
	// clang-format off
	auto list_params = ruis::touch::list::all_parameters{
		.layout_params{
			.dims = {
				ruis::dim(ruis::length::make_pp(100)), //
				ruis::dim(ruis::length::make_pp(100)) //
			}
		},
		.params{
			.specific{
				.provider = std::move(provider)
			}
		}
	};
	// clang-format on

	auto list = utki::make_shared<ruis::touch::list>(c, std::move(list_params));
	list.get().resize(ruis::vec2(100, 100));
	list.get().lay_out();

	return list;
}

} // namespace

namespace {
const tst::set set("touch_list_hover", [](tst::suite& suite) {
	suite.add("mouse_move_events_must_reach_children_of_touch_list_when_it_is_not_scrolling", [] {
		auto c = make_dummy_context();

		auto provider = utki::make_unique<hover_provider>(c);
		auto& provider_ref = provider.get();
		auto list = make_laid_out_list(c, std::move(provider));
		auto& list_w = list.get();

		auto& item = provider_ref.item.get();
		auto& proxy = provider_ref.proxy.get();

		tst::check(!proxy.is_hovered(0), SL);

		// center of the item's mouse_proxy, in the list's coordinates
		auto pos = item.rect().p + proxy.rect().p;
		pos.x() += proxy.rect().d.x() / 2;
		pos.y() += proxy.rect().d.y() / 2;

		list_w.on_mouse_move(ruis::mouse_move_event{
			.pos = pos, //
			.pointer_id = 0, //
			.ignore_mouse_capture = false
		});

		tst::check(proxy.is_hovered(0), SL);

		// move the pointer outside of the item, the proxy should get unhovered
		list_w.on_mouse_move(ruis::mouse_move_event{
			.pos = {pos.x(), pos.y() + 50}, //
			.pointer_id = 0, //
			.ignore_mouse_capture = false
		});

		tst::check(!proxy.is_hovered(0), SL);
	});

	suite.add("mouse_move_event_consumed_by_touch_list_child_must_be_propagated_when_it_is_not_scrolling", [] {
		auto c = make_dummy_context();

		auto provider = utki::make_unique<hover_provider>(c);
		auto& provider_ref = provider.get();
		auto list = make_laid_out_list(c, std::move(provider));
		auto& list_w = list.get();

		provider_ref.proxy.get().mouse_move_handler = [](auto&, auto&) {
			return ruis::event_status::consumed;
		};

		auto& item = provider_ref.item.get();
		auto& proxy = provider_ref.proxy.get();

		// center of the item's mouse_proxy, in the list's coordinates
		auto pos = item.rect().p + proxy.rect().p;
		pos.x() += proxy.rect().d.x() / 2;
		pos.y() += proxy.rect().d.y() / 2;

		auto status = list_w.on_mouse_move(ruis::mouse_move_event{
			.pos = pos, //
			.pointer_id = 0, //
			.ignore_mouse_capture = false
		});

		tst::check(status == ruis::event_status::consumed, SL);
	});
});
} // namespace
