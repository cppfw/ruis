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

#include "tab.hpp"
#include "../../util/util.hpp"

using namespace morda;

bool tab::maskOverlaps(vector2 pos) {
	auto borders = this->get_actual_borders();

	if(pos.x() < borders.left()){
		if(pos.y() != 0 && this->rect().d.y() != 0){
			if(pos.x() / (this->rect().d.y() - pos.y()) < borders.left() / this->rect().d.y()){
				return false;
			}
		}

		if(this->parent() && this->parent()->children().front().get() != this){ // if this is not the first widget in the parent
			auto prevIter = this->parent()->find(*this);
			ASSERT(prevIter != this->parent()->children().end())
			--prevIter;

			if(auto pt = dynamic_cast<tab*>(prevIter->get())){ // previous tab
				if(pt->is_pressed()){
					if(pt->maskOverlaps(pos + this->rect().p - pt->rect().p)){
						return false;
					}
				}
			}
		}
	}else{
		auto rb = this->rect().d.x() - borders.right();
		auto dx = pos.x() - rb;
		if(dx > 0){
			if(pos.y() != 0 && this->rect().d.y() != 0){
				if(dx / pos.y() > borders.right() / this->rect().d.y()){
					return false;
				}
			}
		}
	}
	return true;
}

bool tab::on_mouse_button(const mouse_button_event& e){
	if(e.is_down){
		if(!maskOverlaps(e.pos)){
			return false;
		}
	}
	if(this->nine_patch_button::on_mouse_button(e)){
		return true;
	}
	return this->choice_button::on_mouse_button(e);
}

void tab::on_press_change() {
	this->choice_button::on_press_change();
	this->nine_patch_toggle::on_press_change();
}

tab::tab(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		choice_button(this->context, desc),
		nine_patch_toggle(this->context, desc)
{
	if(!this->get_pressed_nine_patch()){
		this->set_pressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_tab_active"));
	}
	if(!this->get_unpressed_nine_patch()){
		this->set_unpressed_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_tab_inactive"));
	}

	// initialize nine-patch
	this->on_press_change();
}
