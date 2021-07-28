/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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
#include "../proxy/mouse_proxy.hpp"

#include "../../context.hpp"
#include "size_container.hpp"

using namespace morda;

namespace{
class context_menu_wrapper : public size_container{
public:
	context_menu_wrapper(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			size_container(this->context, desc)
	{}
};
}

overlay::overlay(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		pile(this->context, desc)
{}

std::shared_ptr<widget> overlay::show_context_menu(std::shared_ptr<widget> w, vector2 anchor){
	auto c = std::make_shared<context_menu_wrapper>(this->context, treeml::read(R"qwertyuiop(
		layout{
			dx{fill} dy{fill}
		}
		@mouse_proxy{
			layout{
				dx{fill} dy{fill}
			}
			x{0} y{0}
		}
	)qwertyuiop"));

	auto& mp = *std::dynamic_pointer_cast<mouse_proxy>(c->children().back());

	mp.mouse_button_handler = [cntr{utki::make_weak(c)}](mouse_proxy& w, const mouse_button_event& e) -> bool {
		if(auto c = cntr.lock()){
			c->context->run_from_ui_thread([c](){
				c->remove_from_parent();
			});
		}
		return false;
	};

	c->push_back(w);

	auto& lp = c->get_layout_params_const(*w);

	vector2 dim = this->dims_for_widget(*w, lp);

	using std::min;
	using std::max;

	dim = min(dim, this->rect().d); // clamp top

	w->resize(dim);

	anchor = max(anchor, 0); // clamp bottom
	anchor = min(anchor, this->rect().d - w->rect().d); // clamp top

	w->move_to(anchor);

	auto sp = utki::make_shared_from(*this);
	ASSERT(sp)
	this->context->run_from_ui_thread([c, sp](){
		sp->push_back(c);
	});

	return c;
}

void overlay::close_all_context_menus(){
	auto menus = this->get_all_widgets<context_menu_wrapper>();
	for(auto& w : menus){
		w->remove_from_parent();
	}
}
