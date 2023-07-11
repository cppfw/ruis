/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "click_drop_down_box.hpp"

#include <utki/debug.hpp>

#include "../group/overlay.hpp"
#include "../label/color.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace morda;

namespace {
const auto drop_down_box_layout = treeml::read(R"qwertyuiop(
	lp{dx{max} dy{max}}

	@row{
		lp{dx{max}}
		@pile{
			id{morda_dropdown_selection}
			lp{dx{min}dy{max} weight{1}}
		}
		@widget{lp{dx{3dp}}}
		@image{
			image{morda_img_divider_vert}
			lp{dy{fill}}
		}
		@widget{lp{dx{3dp}}}
		@image{
			image{morda_img_dropdown_arrow}
		}
	}
)qwertyuiop");
} // namespace

namespace {
const auto item_layout = treeml::read(R"qwertyuiop(
		@pile{
			lp{
				dx{max}
			}
			@mouse_proxy{
				id{morda_dropdown_mouseproxy}
				lp{
					dx{fill} dy{fill}
				}
			}
			@color{
				id{morda_dropdown_color}
				color{${morda_color_highlight}}
				visible{false}
				lp{
					dx{fill} dy{fill}
				}
			}
		}
	)qwertyuiop");
} // namespace

namespace {
const auto drop_down_menu_layout = treeml::read(R"qwertyuiop(
		@pile{
			@widget{
				id{morda_min_size_forcer}
			}
			@mouse_proxy{
				lp{
					dx{fill} dy{fill}
				}
				id{morda_drop_down_menu_mouse_proxy}
			}
			@nine_patch{
				lp{
					dx{max}
				}
				image{morda_npt_contextmenu_bg}
				@column{
					lp{
						dx{max}
					}
					id{morda_contextmenu_content}
				}
			}
		}
	)qwertyuiop");
} // namespace

click_drop_down_box::click_drop_down_box(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	button(this->context, drop_down_box_layout),
	push_button(this->context, drop_down_box_layout),
	nine_patch_push_button(this->context, drop_down_box_layout),
	drop_down_box(this->context, desc, this->get_widget_as<morda::container>("morda_dropdown_selection"))
{
	this->press_handler = [this](button& b) {
		if (!b.is_pressed()) {
			return;
		}

		this->show_drop_down_menu();
	};
}

bool click_drop_down_box::on_mouse_button(const mouse_button_event& e)
{
	if (e.is_down) {
		++this->num_mouse_buttons_pressed;
	} else {
		ASSERT(this->num_mouse_buttons_pressed != 0)
		--this->num_mouse_buttons_pressed;
	}

	if (!e.is_down) {
		this->handle_mouse_button_up(true);
	}

	return this->nine_patch_push_button::on_mouse_button(e);
}

bool click_drop_down_box::on_mouse_move(const mouse_move_event& e)
{
	if (auto cm = this->current_drop_down_menu.lock()) {
		if (this->num_mouse_buttons_pressed != 0) {
			cm->on_mouse_move(mouse_move_event{
				e.pos + this->get_absolute_pos() - cm->get_absolute_pos(),
				e.pointer_id,
				e.ignore_mouse_capture
			});
		}
	}

	return this->nine_patch_push_button::on_mouse_move(e);
}

void click_drop_down_box::show_drop_down_menu()
{
	if (!this->get_provider()) {
		return;
	}

	auto olay = this->try_get_ancestor<overlay>();
	if (!olay) {
		throw std::logic_error("drop_down_box: no overlay parent found");
	}

	auto np = this->context.get().inflater.inflate(drop_down_menu_layout);

	// force minimum horizontal size of the drop down menu to be the same as the drop down box horizontal size
	{
		// TODO: use get_widget()
		auto min_size_forcer = np.get().try_get_widget("morda_min_size_forcer");

		auto& lp = min_size_forcer->get_layout_params();
		lp.dims.x() = this->rect().d.x();
	}

	// TODO: use get_widget_as()
	auto va = np.get().try_get_widget_as<morda::container>("morda_contextmenu_content");
	ASSERT(va)

	for (size_t i = 0; i != this->get_provider()->count(); ++i) {
		va->push_back(this->wrap_item(this->get_provider()->get_widget(i), i));
	}

	this->hovered_index = -1;

	np.get().get_widget_as<mouse_proxy>("morda_drop_down_menu_mouse_proxy").mouse_button_handler =
		[this](mouse_proxy&, const mouse_button_event& e) -> bool {
		// LOG("button down = " << e.is_down << std::endl)
		if (!e.is_down) {
			this->handle_mouse_button_up(false);
		}

		return true;
	};

	this->current_drop_down_menu =
		olay->show_context_menu(np, this->pos_in_ancestor(vector2(0), olay) + vector2(0, this->rect().d.y()))
			.to_shared_ptr();
}

void click_drop_down_box::handle_mouse_button_up(bool is_first_button_up_event)
{
	auto ddm = this->current_drop_down_menu.lock();
	if (!ddm) {
		return;
	}

	if (this->hovered_index < 0) {
		if (!is_first_button_up_event) {
			this->context.get().run_from_ui_thread([ddm]() {
				ddm->remove_from_parent(); // close drop down menu
			});
		}
		return;
	}
	this->set_selection(this->hovered_index);

	auto ddb = utki::make_shared_from(*this);

	this->context.get().run_from_ui_thread([ddb, ddm]() {
		ddm->remove_from_parent(); // close drop down menu
		if (ddb.get().selection_handler) {
			ddb.get().selection_handler(ddb.get());
		}
	});
}

utki::shared_ref<widget> click_drop_down_box::wrap_item(const utki::shared_ref<widget>& w, size_t index)
{
	auto wd = this->context.get().inflater.inflate_as<morda::container>(item_layout);

	auto mp = wd.get().try_get_widget_as<mouse_proxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd.get().try_get_widget_as<color>("morda_dropdown_color");
	ASSERT(cl)
	auto cl_weak = utki::make_weak(cl);

	wd.get().push_back(w);

	// TODO: which pointer id?
	mp->hover_change_handler = [this, cl_weak, index](mouse_proxy& w, unsigned id) {
		// LOG("hover index = " << index << std::endl)
		if (auto c = cl_weak.lock()) {
			c->set_visible(w.is_hovered(id));
		}
		if (w.is_hovered(id)) {
			this->hovered_index = int(index);
			// LOG("this->hovered_index = " << this->hovered_index << std::endl)
		} else {
			if (this->hovered_index >= 0 && decltype(index)(this->hovered_index) == index) {
				// LOG("this->hovered_index = -1;" << std::endl)
				this->hovered_index = -1;
			}
		}
	};

	return wd;
}
