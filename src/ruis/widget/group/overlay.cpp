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
	popup_wrapper(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		container(this->context, desc, layout::size)
	{}
};
} // namespace

overlay::overlay(
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	utki::span<const utki::shared_ref<widget>> children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	container( //
		this->context,
		{.container_params = {.layout = layout::pile}},
		children
	)
{}

overlay::overlay(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	container(this->context, desc, layout::pile)
{}

utki::shared_ref<widget> overlay::show_popup(const utki::shared_ref<widget>& w, vector2 anchor)
{
	auto c = utki::make_shared<popup_wrapper>(this->context, tml::read(R"qwertyuiop(
		lp{
			dx{fill} dy{fill}
		}
		@mouse_proxy{
			lp{
				dx{fill} dy{fill}
			}
			x{0} y{0}
		}
	)qwertyuiop"));

	// TODO: rewrite using utki::dynamic_reference_cast()
	auto& mp = *std::dynamic_pointer_cast<mouse_proxy>(c.get().children().back().to_shared_ptr());

	mp.mouse_button_handler = [cntr{utki::make_weak(c)}](mouse_proxy& w, const mouse_button_event& e) -> bool {
		if (auto c = cntr.lock()) {
			c->context.get().post_to_ui_thread([c]() {
				c->remove_from_parent();
			});
		}
		return false;
	};

	c.get().push_back(w);

	vector2 dim = dims_for_widget(w.get(), this->rect().d);

	using std::min;
	using std::max;

	dim = min(dim, this->rect().d); // clamp top

	w.get().resize(dim);

	anchor = max(anchor, 0); // clamp bottom
	anchor = min(anchor, this->rect().d - w.get().rect().d); // clamp top

	w.get().move_to(anchor);

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
